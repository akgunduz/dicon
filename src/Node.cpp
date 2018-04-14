//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "ExecutorItem.h"

Node::Node(const char *rootPath) :
        Component(COMP_NODE, rootPath) {

	LOG_U(UI_UPDATE_NODE_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_COLLECTOR));
	LOG_U(UI_UPDATE_NODE_STATE, IDLE);

	distributorAddress = 0;
}

Node::~Node() {

}

bool Node::processDistributorMsg(long address, Message *msg) {

    bool status = false;

    switch(msg->getHeader()->getType()) {

        case MSGTYPE_WAKEUP:

            setDistributorAddress(address);

            status = send2DistributorMsg(address, MSGTYPE_ALIVE);
            break;

        default :
            break;
    }

    delete msg;
    return status;
}

bool Node::processCollectorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getHeader()->getType()) {

		case MSGTYPE_JOB: {

            LOG_U(UI_UPDATE_NODE_STATE, BUSY);
            status = send2DistributorMsg(distributorAddress, MSGTYPE_BUSY);

            LOG_U(UI_UPDATE_NODE_CLEAR, "");

            setExecutor(msg->getData()->getExecutor());

            TypeFileList *list = msg->getData()->getFileList();

            TypeFileList requiredList = checkFileExistence(*list);
            if (requiredList.size()) {

                status &= send2CollectorMsg(address, MSGTYPE_INFO, &requiredList);

            } else {

                processCommand(getExecutor());

                LOG_U(UI_UPDATE_NODE_STATE, IDLE);
                //TODO will update with md5s including outputs
                status = send2DistributorMsg(distributorAddress, MSGTYPE_READY);
            }


        }
			break;

		case MSGTYPE_BINARY: {

            processCommand(getExecutor());

            LOG_U(UI_UPDATE_NODE_STATE, IDLE);
            //TODO will update with md5s including outputs
            status = send2DistributorMsg(distributorAddress, MSGTYPE_READY);
        }
			break;

		default :
			break;
	}

	delete msg;
	return status;
}

bool Node::processNodeMsg(long address, Message *msg) {
	return false;
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

            //buraya ek md5 karsilastirmasi eklenecek
            reqList.push_back(list[i]);
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
   // char cmd[PATH_MAX] = "";
    char *args[100];

    LOG_U(UI_UPDATE_NODE_LOG, "Executing %s command", cmd);
    LOG_U(UI_UPDATE_NODE_EXEC_LIST, cmd);

    char fullcmd[PATH_MAX];
    strcpy(fullcmd, Util::getAbsPath(getHost(), cmd).c_str());

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

bool Node::processParallel(Job* job) {

//    int status;
//    char cmd[PATH_MAX] = "";
//    char *args[100];
//
//    for (int i = 0; i < job->getContentCount(CONTENT_EXECUTOR); i++) {
//        ExecutorItem *content = (ExecutorItem *) job->getContent(CONTENT_EXECUTOR, i);
//        content->parse(job, cmd);
//        parseCommand(cmd, args);
//        LOG_U(UI_UPDATE_NODE_LOG, "Executing %s command", cmd + strlen(Unit::getRootPath(COMP_NODE)));
//        LOG_U(UI_UPDATE_NODE_EXEC_LIST, cmd);
//
//#ifdef CYGWIN
//        LOG_I("Simulating fork in Windows!!!");
//#else
//
//        pid_t pid = fork();
//
//        if (pid == -1) {
//            LOG_E("Rule Process failed in fork!!!");
//            return false;
//
//        } else if (pid == 0) {
//            //child part
//            execv(*args, args);
//            LOG_E("ExecV failed with error : %d", errno);
//            exit(EXIT_FAILURE);
//
//        }
//#endif
//    }
//
//#ifdef CYGWIN
//    LOG_I("Simulating wait in Windows!!!");
//#else
//    while (true) {
//
//        pid_t pid = wait(&status);
//        if (pid == -1) {
//            if (errno == ECHILD) {
//                //no more child process
//                break;
//            }
//        } else {
//            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
//                LOG_E("Process with pid %d failed in fork!!!", pid);
//                return false;
//            }
//        }
//    }
//#endif

    return true;
}

bool Node::processSequential(Job* job) {

//    int status;
//    char cmd[PATH_MAX] = "";
//    char *args[100];
//
//    for (int i = 0; i < job->getContentCount(CONTENT_EXECUTOR); i++) {
//        ExecutorItem *content = (ExecutorItem *) job->getContent(CONTENT_EXECUTOR, i);
//
//        content->parse(job, cmd);
//
//        LOG_U(UI_UPDATE_NODE_LOG, "Executing %s command", cmd);
//        LOG_U(UI_UPDATE_NODE_EXEC_LIST, cmd);
//
//        char fullcmd[PATH_MAX];
//        strcpy(fullcmd, Util::absPath(getHost(), cmd).c_str());
//
//        parseCommand(fullcmd, args);
//
//
//#ifdef CYGWIN
//        LOG_I("Simulating fork in Windows!!!");
//#else
//
//        pid_t pid = fork();
//
//        if (pid == -1) {
//            LOG_E("Rule Process failed in fork!!!");
//            return false;
//
//        } else if (pid > 0) {
//            //parent part
//            int res;
//            do {
//                //res = wait(&status);
//                res = waitpid(pid, &status, 0);
//            } while ((res < 0) && (errno == EINTR));
//
//        } else {
//            //child part
//            execv(*args, args);
//            LOG_E("ExecV failed with error : %d for command %s", errno, cmd);
//            exit(EXIT_FAILURE);
//        }
//#endif
//    }

    return true;
}
