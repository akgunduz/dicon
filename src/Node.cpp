//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "ExecutorItem.h"
#include "NodeState.h"
#include "CollectorObject.h"


Node *Node::newInstance(const char* path) {

    return new Node(path);
}

Node::Node(const char *rootPath) :
        Component(COMP_NODE, rootPath), distributor(COMP_DISTRIBUTOR, getRootPath()){

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_PROCESS] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_PROCESS] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg);
}

Node::~Node() {

}

bool Node::processDistributorWakeupMsg(ComponentObject owner, Message *msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(ComponentObject owner, Message *msg) {

    setHostID((int)msg->getHeader()->getVariant(0));

    LOG_U(UI_UPDATE_NODE_ID, std::vector<long>{getHost().getID()});
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processDistributorProcessMsg(ComponentObject owner, Message *msg) {

    int collID = (int)msg->getHeader()->getVariant(0);
    long collAddress = msg->getHeader()->getVariant(1);

    if (!getData()->getJobID().equal(msg->getData()->getJobID())) {
        LOGS_E(getHost(), "Wrong Job ID came from Distributor, it should not!!!");
        delete msg;
        return false;
    }

    TypeFileInfoList requiredList = FileInfo::checkFileExistence(getHost(), getData()->getFileList());
    if (!requiredList.empty()) {

        return send2CollectorInfoMsg(CollectorObject(collID, collAddress),
                                     getData()->getJobID(),
                                     getData()->getJobDir(),
                                     getData()->getProcessID(),
                                     &requiredList);

    } else {

        processCommand(collID,
                       getData()->getJobID(),
                       getData()->getJobDir(),
                       getData()->getProcessID(),
                       getData()->getProcessCommand());

        TypeFileInfoList outputList = FileInfo::getFileList(getData()->getFileList(), true);
        FileInfo::setFileListState(&outputList, false);

        //TODO will update with md5s including outputs
        //TODO will update with actual binary to collector including outputs
        return send2CollectorBinaryMsg(CollectorObject(collID, collAddress),
                                       getData()->getJobID(),
                                       getData()->getJobDir(),
                                       getData()->getProcessID(),
                                       &outputList);
    }
}

bool Node::processCollectorJobMsg(ComponentObject owner, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {BUSY});

    getData()->reset();
    getData()->set(msg->getData());

    return send2DistributorBusyMsg(getDistributor(),
                                   msg->getData()->getJobID(),
                                   owner.getAddress());
}

bool Node::processCollectorBinaryMsg(ComponentObject owner, Message *msg) {

    processCommand(msg->getHeader()->getOwner().getID(),
                   getData()->getJobID(),
                   getData()->getJobDir(),
                   getData()->getProcessID(),
                   getData()->getProcessCommand());

    TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), true);
    FileInfo::setFileListState(&outputList, false);
    //TODO will update with md5s including outputs
    //TODO will update with actual binary to collector including outputs
    return send2CollectorBinaryMsg(owner,
                                   getData()->getJobID(),
                                   getData()->getJobDir(),
                                   getData()->getProcessID(),
                                   &outputList);
}

bool Node::processCollectorReadyMsg(ComponentObject owner, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {IDLE});

    return send2DistributorReadyMsg(getDistributor(), getData()->getJobID(), owner.getAddress());
}

bool Node::send2DistributorReadyMsg(ComponentObject target, Uuid jobID, long collAddress) {

	auto *msg = new Message(getHost(), MSGTYPE_READY);

    msg->getHeader()->setVariant(0, collAddress);

    msg->getData()->setJobID(jobID);

	return send(target, msg);
}

bool Node::send2DistributorAliveMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Node::send2DistributorIDMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    return send(target, msg);
}

bool Node::send2DistributorBusyMsg(ComponentObject target, Uuid jobID, long collAddress) {

    auto *msg = new Message(getHost(), MSGTYPE_BUSY);

    msg->getHeader()->setVariant(0, collAddress);

    msg->getData()->setJobID(jobID);

    return send(target, msg);
}

bool Node::send2CollectorInfoMsg(ComponentObject target, Uuid jobID,
                                 const char* jobDir, long processID, TypeFileInfoList *fileList) {

	auto *msg = new Message(getHost(), MSGTYPE_INFO);

    msg->getData()->setJob(jobID, jobDir)
            .setProcessID(processID)
            .addFileList(fileList,false);

	return send(target, msg);
}

bool Node::send2CollectorBinaryMsg(ComponentObject target, Uuid jobID, const char* jobDir, long processID,
                                   TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setJob(jobID, jobDir)
            .setProcessID(processID)
            .addFileList(fileList,true);

    return send(target, msg);
}

ComponentObject Node::getDistributor() {

    return distributor;
}

void Node::setDistributor(ComponentObject distributor) {

    this->distributor = distributor;
}

void Node::parseCommand(char *cmd, char **argv) {

    while (*cmd != '\0') {

        while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') {
            *cmd++ = '\0';
        }

        *argv++ = cmd;

        while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n') {
            cmd++;
        }
    }
    *argv = nullptr;
}

bool Node::processCommand(int collID, Uuid jobID, const char* jobDir, long processID, const char *cmd) {

    int status;
    char *args[100];

    char fullcmd[PATH_MAX];

    strcpy(fullcmd, Util::parsePath(getHost().getRootPath(), cmd).c_str());

    LOG_U(UI_UPDATE_NODE_PROCESS_LIST, collID, processID, jobID.getStr().c_str(), jobDir);
    LOGS_I(getHost(), "Collector[%d]\'s Process[%d] is executing", collID, processID);
    LOGS_T(getHost(), "Command : %s", fullcmd);

    parseCommand(fullcmd, args);

    pid_t pid = fork();

    if (pid == -1) {
        LOGS_E(getHost(), "Job Process failed in fork!!!");
        return false;

    } else if (pid > 0) {
        //parent part
        int res;
        do {
            res = waitpid(pid, &status, 0);
        } while ((res < 0) && (errno == EINTR));

    } else {
        //child part
        execv(*args, args);
        LOGS_E(getHost(), "ExecV failed with error : %d for command %s", errno, cmd);
        exit(EXIT_FAILURE);
    }

    return true;

}

MessageData *Node::getData() {

    return &data;
}
