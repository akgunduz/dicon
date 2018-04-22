//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "ExecutorItem.h"
#include "NodeState.h"

Node::Node(const char *rootPath) :
        Component(COMP_NODE, rootPath) {

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_JOB] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);

	LOG_U(UI_UPDATE_NODE_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_COLLECTOR));
	LOG_U(UI_UPDATE_NODE_STATE, IDLE);

	setDistributorAddress(0);
}

Node::~Node() {

}

bool Node::processDistributorWakeupMsg(long address, Message *msg) {

    setDistributorAddress(address);

    return send2DistributorMsg(address, MSGTYPE_ALIVE);
}

bool Node::processCollectorJobMsg(long address, Message *msg) {

    LOG_U(UI_UPDATE_NODE_STATE, BUSY);
    LOG_U(UI_UPDATE_NODE_CLEAR, "");

    if (!send2DistributorMsg(getDistributorAddress(), MSGTYPE_BUSY)) {

        LOG_E("Could not send BUSY message to Distributor!!!");
        return false;
    }

    setExecutor(msg->getData()->getExecutor());

    TypeFileList *list = msg->getData()->getFileList();

    TypeFileList requiredList = checkFileExistence(*list);
    if (!requiredList.empty()) {

        return send2CollectorMsg(address, MSGTYPE_INFO, &requiredList);

    } else {

        processCommand(getExecutor());

        LOG_U(UI_UPDATE_NODE_STATE, IDLE);
        //TODO will update with md5s including outputs
        return send2DistributorMsg(getDistributorAddress(), MSGTYPE_READY);
    }
}

bool Node::processCollectorBinaryMsg(long address, Message *msg) {

    processCommand(getExecutor());

    LOG_U(UI_UPDATE_NODE_STATE, IDLE);
    //TODO will update with md5s including outputs
    return send2DistributorMsg(getDistributorAddress(), MSGTYPE_READY);
}

bool Node::send2DistributorMsg(long address, MSG_TYPE type, ...) {

	Message *msg = new Message(COMP_NODE, type);

	switch(type) {

		case MSGTYPE_READY:
		case MSGTYPE_ALIVE:
		case MSGTYPE_BUSY:
			break;

		default:
			delete msg;
			return false;
	}

	return send(COMP_DISTRIBUTOR, address, msg);

}

bool Node::send2CollectorMsg(long address, MSG_TYPE type, ...) {

    va_list ap;
    va_start(ap, type);

	Message *msg = new Message(COMP_NODE, type);

	switch(type) {

		case MSGTYPE_INFO: {
            TypeFileList *fileList = va_arg(ap, TypeFileList*);
            msg->getData()->setStreamFlag(STREAM_INFO);
            msg->getData()->addFileList(fileList);
            LOG_U(UI_UPDATE_NODE_LOG, "\"%d\" file info is prepared", fileList->size());
        }
			break;

		default:
			delete msg;
            va_end(ap);
			return false;
	}

    va_end(ap);

	return send(COMP_COLLECTOR, address, msg);
}

const char* Node::getExecutor() {

    return executor;
}

void Node::setExecutor(char *executor) {

    strcpy(this->executor, executor);
}

long Node::getDistributorAddress() {

    return distributorAddress;
}

void Node::setDistributorAddress(long distributorAddress) {

    this->distributorAddress = distributorAddress;
}

TypeFileList Node::checkFileExistence(TypeFileList list) {

    TypeFileList reqList;

    for (int i = 0; i < list.size(); i++) {

        if (!Util::checkPath(ComponentTypes::getRootPath(COMP_NODE),
                             list[i]->getJobDir(), list[i]->getFileName(), false)) {

            reqList.push_back(list[i]);

        } else {

            Md5 md5(Util::getAbsMD5Path(COMP_NODE, list[i]->getJobDir(), list[i]->getFileName()).c_str());
            if (!md5.equal(list[i]->getMD5())) {

                reqList.push_back(list[i]);
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
            //res = wait(&status);
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
