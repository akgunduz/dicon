//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "ExecutorItem.h"
#include "NodeState.h"


Node *Node::newInstance(const char* path) {

    return new Node(path);
}

Node::Node(const char *rootPath) :
        Component(COMP_NODE, rootPath) {

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_JOB] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg);

	LOG_U(UI_UPDATE_NODE_ADDRESS, std::vector<long> {getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_COLLECTOR)});
	LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {IDLE});

    setDistributorAddress(0);
}

Node::~Node() {

}

bool Node::processDistributorWakeupMsg(ComponentObject owner, long address, Message *msg) {

    setDistributorAddress(address);

    return send2DistributorAliveMsg(address);
}

bool Node::processDistributorIDMsg(ComponentObject owner, long address, Message *msg) {

    setHostID((int)msg->getHeader()->getVariant(0));

    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(address);
}

bool Node::processCollectorJobMsg(ComponentObject owner, long address, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {BUSY});

    if (!send2DistributorBusyMsg(getDistributorAddress())) {

        LOGS_E(getHost(), "Could not send BUSY message to Distributor!!!");
        return false;
    }

    TypeFileInfoList requiredList = FileInfo::checkFileExistence(getHost(), msg->getData()->getFileList());
    if (!requiredList.empty()) {

        return send2CollectorInfoMsg(address, msg->getData()->getJobDir(), msg->getData()->getExecutorID(),
                                     msg->getData()->getExecutor(), &requiredList);

    } else {

        processCommand(msg->getData()->getExecutor());

        TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), true);
        FileInfo::setFileListState(&outputList, false);

        //TODO will update with md5s including outputs
        //TODO will update with actual binary to collector including outputs
        return send2CollectorBinaryMsg(address, msg->getData()->getJobDir(), msg->getData()->getExecutorID(),
                                       msg->getData()->getExecutor(), &outputList);
    }
}

bool Node::processCollectorBinaryMsg(ComponentObject owner, long address, Message *msg) {

    processCommand(msg->getData()->getExecutor());

    TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), true);
    FileInfo::setFileListState(&outputList, false);
    //TODO will update with md5s including outputs
    //TODO will update with actual binary to collector including outputs
    return send2CollectorBinaryMsg(address, msg->getData()->getJobDir(), msg->getData()->getExecutorID(),
                                   msg->getData()->getExecutor(), &outputList);
}

bool Node::processCollectorReadyMsg(ComponentObject owner, long address, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, std::vector<long> {IDLE});

    return send2DistributorReadyMsg(getDistributorAddress());
}

bool Node::send2DistributorReadyMsg(long address) {

	auto *msg = new Message(getHost(), MSGTYPE_READY);

	return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2DistributorAliveMsg(long address) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2DistributorIDMsg(long address) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2DistributorBusyMsg(long address) {

    auto *msg = new Message(getHost(), MSGTYPE_BUSY);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2CollectorInfoMsg(long address, const char* jobDir, long executorID,
                                 const char* executor, TypeFileInfoList *fileList) {

	auto *msg = new Message(getHost(), MSGTYPE_INFO);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executorID, executor);
    msg->getData()->addFileList(fileList);

    LOGS_I(getHost(), "\"%d\" file info is prepared", fileList->size());

	return send(COMP_COLLECTOR, address, msg);
}

bool Node::send2CollectorBinaryMsg(long address, const char* jobDir, long executorID,
                                   const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executorID, executor);
    msg->getData()->addFileList(fileList);

    LOGS_I(getHost(), "\"%d\" file binary is prepared", fileList->size());

    return send(COMP_COLLECTOR, address, msg);
}

long Node::getDistributorAddress() {

    return distributorAddress;
}

void Node::setDistributorAddress(long distributorAddress) {

    this->distributorAddress = distributorAddress;
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

bool Node::processCommand(const char *cmd) {

    int status;
    char *args[100];

    char fullcmd[PATH_MAX];

    strcpy(fullcmd, Util::parsePath(getHost(), cmd).c_str());

    LOG_U(UI_UPDATE_NODE_EXEC_LIST, fullcmd);
    LOGS_I(getHost(), "Executing %s command\n", fullcmd);

    parseCommand(fullcmd, args);


#ifdef CYGWIN
    LOG_I("Simulating fork in Windows!!!");
#else

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
#endif

    return true;

}
