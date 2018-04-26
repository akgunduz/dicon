//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "ExecutorItem.h"
#include "NodeState.h"


Node *Node::newInstance(int id) {

    char path[PATH_MAX];
    sprintf(path, "%s/%s_%d", getcwd(NULL, 0), NODE_PATH, id);
    mkdir(path, 0777);

    auto *instance = new Node(path);

    return instance;
}

Node::Node(const char *rootPath) :
        Component(COMP_NODE, rootPath) {

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_JOB] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg);

	LOG_U(UI_UPDATE_NODE_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_COLLECTOR));
	LOG_U(UI_UPDATE_NODE_STATE, IDLE);

	setDistributorAddress(0);
}

Node::~Node() {

}

bool Node::processDistributorWakeupMsg(long address, Message *msg) {

    setDistributorAddress(address);

    return send2DistributorAliveMsg(address);
}

bool Node::processDistributorIDMsg(long address, Message *msg) {

    setID((int)msg->getHeader()->getVariant(0));

    LOG_U(UI_UPDATE_NODE_LOG, "New ID : %d is assigned by Distributor", getID());

    return send2DistributorIDMsg(address);
}

bool Node::processCollectorJobMsg(long address, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, BUSY);
    LOG_U(UI_UPDATE_NODE_CLEAR, "");

    if (!send2DistributorBusyMsg(getDistributorAddress())) {

        LOG_E("Could not send BUSY message to Distributor!!!");
        return false;
    }

    TypeFileInfoList list = FileInfo::getFileList(msg->getData()->getFileList(), FILEINFO_ALL);

    TypeFileInfoList requiredList = checkFileExistence(&list);
    if (!requiredList.empty()) {

        return send2CollectorInfoMsg(address, msg->getData()->getJobDir(),
                                     msg->getData()->getExecutor(), &requiredList);

    } else {

        processCommand(msg->getData()->getExecutor());

        TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), FILEINFO_OUTPUT);
        FileInfo::setFileListState(&outputList, FILEINFO_EXIST);

        //TODO will update with md5s including outputs
        //TODO will update with actual binary to collector including outputs
        return send2CollectorBinaryMsg(address, msg->getData()->getJobDir(),
                                       msg->getData()->getExecutor(), &outputList);
    }
}

bool Node::processCollectorBinaryMsg(long address, Message *msg) {

    processCommand(msg->getData()->getExecutor());

    TypeFileInfoList outputList = FileInfo::getFileList(msg->getData()->getFileList(), FILEINFO_OUTPUT);
    FileInfo::setFileListState(&outputList, FILEINFO_EXIST);
    //TODO will update with md5s including outputs
    //TODO will update with actual binary to collector including outputs
    return send2CollectorBinaryMsg(address, msg->getData()->getJobDir(),
                                   msg->getData()->getExecutor(), &outputList);
}

bool Node::processCollectorReadyMsg(long address, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, IDLE);

    return send2DistributorReadyMsg(getDistributorAddress());
}

bool Node::send2DistributorReadyMsg(long address) {

	auto *msg = new Message(COMP_NODE, MSGTYPE_READY);

	return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2DistributorAliveMsg(long address) {

    auto *msg = new Message(COMP_NODE, MSGTYPE_ALIVE);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2DistributorIDMsg(long address) {

    auto *msg = new Message(COMP_NODE, MSGTYPE_ID);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2DistributorBusyMsg(long address) {

    auto *msg = new Message(COMP_NODE, MSGTYPE_BUSY);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Node::send2CollectorInfoMsg(long address, const char* jobDir, const char* executor, TypeFileInfoList *fileList) {

	auto *msg = new Message(COMP_NODE, MSGTYPE_INFO);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executor);
    msg->getData()->addFileList(fileList);

    LOG_U(UI_UPDATE_NODE_LOG, "\"%d\" file info is prepared", fileList->size());

	return send(COMP_COLLECTOR, address, msg);
}

bool Node::send2CollectorBinaryMsg(long address, const char* jobDir, const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(COMP_NODE, MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executor);
    msg->getData()->addFileList(fileList);

    LOG_U(UI_UPDATE_NODE_LOG, "\"%d\" file binary is prepared", fileList->size());

    return send(COMP_COLLECTOR, address, msg);
}

long Node::getDistributorAddress() {

    return distributorAddress;
}

void Node::setDistributorAddress(long distributorAddress) {

    this->distributorAddress = distributorAddress;
}

TypeFileInfoList Node::checkFileExistence(TypeFileInfoList *list) {

    TypeFileInfoList reqList;

    for (int i = 0; i < list->size(); i++) {

        if (!Util::checkPath(ComponentTypes::getRootPath(COMP_NODE),
                             list->at(i).get()->getJobDir(), list->at(i).get()->getFileName(), false)) {

            reqList.push_back(list->at(i));

        } else {

            Md5 md5(Util::getAbsMD5Path(COMP_NODE,
                                        list->at(i).get()->getJobDir(), list->at(i).get()->getFileName()).c_str());
            if (!md5.equal(list->at(i).get()->getMD5())) {

                reqList.push_back(list->at(i));
            }
        }
    }

    return reqList;
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

    LOG_U(UI_UPDATE_NODE_LOG, "Executing %s command", fullcmd);
    LOG_U(UI_UPDATE_NODE_EXEC_LIST, fullcmd);

    parseCommand(fullcmd, args);


#ifdef CYGWIN
    LOG_I("Simulating fork in Windows!!!");
#else

    pid_t pid = fork();

    if (pid == -1) {
        LOG_E("Job Process failed in fork!!!");
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
        LOG_E("ExecV failed with error : %d for command %s", errno, cmd);
        exit(EXIT_FAILURE);
    }
#endif

    return true;

}
