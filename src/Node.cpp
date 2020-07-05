//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "NodeState.h"
#include "CollectorObject.h"


Node *Node::newInstance(const char* path) {

    return new Node(path);
}

Node::Node(const char *rootPath) :
        Component(rootPath), distributor(getRootPath()){

    host = new NodeObject(getRootPath());

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_PROCESS] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_JOB] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg);

    initInterfaces(COMP_NODE);
}

Node::~Node() {

}

bool Node::processDistributorWakeupMsg(ComponentObject owner, Message *msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(ComponentObject owner, Message *msg) {

    getHost().setID((int)msg->getHeader()->getVariant(0));

    LOG_U(UI_UPDATE_NODE_ID, std::vector<long>{getHost().getID()});
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processDistributorProcessMsg(ComponentObject owner, Message *msg) {

    int collID = (int)msg->getHeader()->getVariant(0);
    long collAddress = msg->getHeader()->getVariant(1);

    TypeFileInfoList requiredList = FileInfo::checkFileExistence(getHost(), msg->getData()->getFileList());
    if (!requiredList.empty()) {

        LOGS_I(getHost(), "Missing files for Process[%d] is requested from Collector[%d]",
                msg->getData()->getExecutorID(), collID);

        return send2CollectorInfoMsg(CollectorObject(collID, collAddress),
                                     msg->getData()->getJobDir(),
                                     msg->getData()->getExecutorID(),
                                     msg->getData()->getExecutor(), &requiredList);

    } else {

//        ((NodeObject&)getHost()).getProcess().setAttachedColl(collID);
//        ((NodeObject&)getHost()).getProcess().setProcessID(msg->getData()->getExecutorID());
//        ((NodeObject&)getHost()).getProcess().setJobID(msg->getData()->getJobDir());
//        processList.emplace_back(((NodeObject&)getHost()).getProcess());

        LOGS_I(getHost(), "Process[%d] from Collector[%d] is executing",
               msg->getData()->getExecutorID(), collID);

        LOG_U(UI_UPDATE_NODE_PROCESS_LIST, collID,
              msg->getData()->getJobDir(), msg->getData()->getExecutorID());

        processCommand(msg->getData()->getExecutorID(), msg->getData()->getExecutor());

        TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), true);
        FileInfo::setFileListState(&outputList, false);

        LOGS_I(getHost(), "Process[%d] from Collector[%d] is executed, sending back output data",
               msg->getData()->getExecutorID(), collID);

        //TODO will update with md5s including outputs
        //TODO will update with actual binary to collector including outputs
        return send2CollectorBinaryMsg(CollectorObject(collID, collAddress),
                                       msg->getData()->getJobDir(),
                                       msg->getData()->getExecutorID(),
                                       msg->getData()->getExecutor(), &outputList);
    }
}

bool Node::processCollectorJobMsg(ComponentObject owner, Message *msg) {

    ((NodeObject&)getHost()).setState(NODESTATE_BUSY);

    LOGS_I(getHost(), "Process[%d] request is received from Collector[%d]",
           msg->getData()->getExecutorID(), owner.getID());

    LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {NODESTATE_BUSY});

    LOGS_I(getHost(), "Sending Busy message to distributor");

    return send2DistributorBusyMsg(getDistributor(),
                                   msg->getData()->getJobDir(),
                                   msg->getData()->getExecutorID(),
                                   msg->getData()->getExecutor(),
                                   msg->getData()->getFileList(),
                                   owner.getAddress());
}

bool Node::processCollectorBinaryMsg(ComponentObject owner, Message *msg) {

//    ((NodeObject&)getHost()).getProcess().setAttachedColl(msg->getHeader()->getOwner().getID());
//    ((NodeObject&)getHost()).getProcess().setProcessID(msg->getData()->getExecutorID());
//    ((NodeObject&)getHost()).getProcess().setJobID(msg->getData()->getJobDir());
//    processList.emplace_back(((NodeObject&)getHost()).getProcess());

    LOGS_I(getHost(), "Process[%d] binaries are received from Collector[%d]",
           msg->getData()->getExecutorID(), owner.getID());

    LOG_U(UI_UPDATE_NODE_PROCESS_LIST, msg->getHeader()->getOwner().getID(),
            msg->getData()->getJobDir(), msg->getData()->getExecutorID());

    LOGS_I(getHost(), "Process[%d] from Collector[%d] is executing",
           msg->getData()->getExecutorID(), owner.getID());

    processCommand(msg->getData()->getExecutorID(), msg->getData()->getExecutor());

    TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), true);
    FileInfo::setFileListState(&outputList, false);

    LOGS_I(getHost(), "Process[%d] from Collector[%d] is executed, sending back output data",
           msg->getData()->getExecutorID(), owner.getID());

    //TODO will update with md5s including outputs
    //TODO will update with actual binary to collector including outputs
    return send2CollectorBinaryMsg(owner, msg->getData()->getJobDir(), msg->getData()->getExecutorID(),
                                   msg->getData()->getExecutor(), &outputList);
}

bool Node::processCollectorReadyMsg(ComponentObject owner, Message *msg) {

    ((NodeObject&)getHost()).setState(NODESTATE_IDLE);

    LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {NODESTATE_IDLE});

    long collUnservedCount = msg->getHeader()->getVariant(0);

    return send2DistributorReadyMsg(getDistributor(), msg->getData()->getJobDir(), owner.getAddress(), collUnservedCount);
}

bool Node::send2DistributorReadyMsg(ComponentObject target, const char* jobDir, long collAddress, long collUnservedCount) {

	auto *msg = new Message(getHost(), MSGTYPE_READY);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getHeader()->setVariant(0, collAddress);
    msg->getHeader()->setVariant(1, collUnservedCount);
    msg->getData()->setJobDir(jobDir);

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

bool Node::send2DistributorBusyMsg(ComponentObject target,
                                   const char* jobDir, long executionID, const char *executor,
                                   TypeFileInfoList *fileList, long collAddress) {

    auto *msg = new Message(getHost(), MSGTYPE_BUSY);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executionID, executor);
    msg->getData()->addFileList(fileList);
    msg->getHeader()->setVariant(0, collAddress);

    return send(target, msg);
}

bool Node::send2CollectorInfoMsg(ComponentObject target, const char* jobDir, long executorID,
                                 const char* executor, TypeFileInfoList *fileList) {

	auto *msg = new Message(getHost(), MSGTYPE_INFO);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executorID, executor);
    msg->getData()->addFileList(fileList);

	return send(target, msg);
}

bool Node::send2CollectorBinaryMsg(ComponentObject target, const char* jobDir, long executorID,
                                   const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executorID, executor);
    msg->getData()->addFileList(fileList);

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

bool Node::processCommand(int processID, const char *cmd) {

    int status;
    char *args[100];

    char fullcmd[PATH_MAX];

    strcpy(fullcmd, Util::parsePath(getHost().getRootPath(), cmd).c_str());

    LOGS_T(getHost(), "Process[%d] is started", processID);
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

    LOGS_T(getHost(), "Process[%d] is ended", processID);

    return true;
}

std::vector<NodeProcessInfo> *Node::getProcessList() {

    return &processList;
}
