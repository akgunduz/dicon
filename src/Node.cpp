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

            TypeMD5List *list = msg->getData()->getMD5List();

            TypeMD5List existList = checkFileExistence(list);

            status &= send2CollectorMsg(address, MSGTYPE_MD5, &existList);
        }
			break;

		case MSGTYPE_BINARY: {

            processRule();

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

		case MSGTYPE_MD5: {
            TypeMD5List *md5List = va_arg(ap, TypeMD5List*);
            msg->getData()->setStreamFlag(STREAM_MD5ONLY);
            msg->getData()->addMD5List(md5List);
            LOG_U(UI_UPDATE_NODE_LOG, "\"%d\" file md5 is prepared", md5List->size());
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

bool Node::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}

TypeMD5List Node::checkFileExistence(TypeMD5List *) {


    return TypeMD5List();
}

bool Node::processMD5() {

//
//    for (int i = 0; i < job->getContentCount(CONTENT_FILE); i++) {
//        FileItem *content = (FileItem *)job->getContent(CONTENT_FILE, i);
//
//        /*
//         * false -> request file from collector, with no md5 set
//         * true -> do not request file from collector, with md5 set
//         */
//        if (!Util::checkPath(Unit::getRootPath(COMP_NODE) ,content->getFileName(), false)) {
//            content->setFlaggedToSent(false);
//        }
//    }


	return true;

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

bool Node::processRule() {

    int status;
    char cmd[PATH_MAX] = "";
    char *args[100];

    LOG_U(UI_UPDATE_NODE_LOG, "Executing %s command", cmd);
    LOG_U(UI_UPDATE_NODE_EXEC_LIST, cmd);

    char fullcmd[PATH_MAX];
    strcpy(fullcmd, Util::absPath(getHost(), cmd).c_str());

    parseCommand(fullcmd, args);


#ifdef CYGWIN
    LOG_I("Simulating fork in Windows!!!");
#else

    pid_t pid = fork();

    if (pid == -1) {
        LOG_E("Rule Process failed in fork!!!");
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

void Node::setExecutor(char *executor) {

    strcpy(this->executor, executor);
}


