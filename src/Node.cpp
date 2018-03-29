//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"

Node::Node(const char *rootPath) :
        Component(Unit(COMP_NODE), rootPath) {

	LOG_U(UI_UPDATE_NODE_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_COLLECTOR));
	LOG_U(UI_UPDATE_NODE_STATE, IDLE);

	distributorAddress = 0;

	LOG_I("Instance is created!!!");

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

		case MSGTYPE_RULE:

			LOG_U(UI_UPDATE_NODE_STATE, BUSY);
			status = send2DistributorMsg(distributorAddress, MSGTYPE_BUSY);

			LOG_U(UI_UPDATE_NODE_CLEAR, "");

            if (job != nullptr) {
                delete job;
            }

            job = new Job(getHost(), msg->getJobDir());

			if (!processMD5()) {
				LOG_E("Processing MD5 failed!!!");
				break;
			}

			status &= send2CollectorMsg(address, MSGTYPE_MD5);
			break;

		case MSGTYPE_BINARY:

            for (int i = 0; i < job->getRuleCount(); i++) {
                processRule(job->getRule(i));
            }

			LOG_U(UI_UPDATE_NODE_STATE, IDLE);
			status = send2DistributorMsg(distributorAddress, MSGTYPE_READY);
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

bool Node::send2DistributorMsg(long address, MSG_TYPE type) {

	Message *msg = new Message(Unit(COMP_NODE), type);

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

bool Node::send2CollectorMsg(long address, MSG_TYPE type) {

	Message *msg = new Message(COMP_NODE, type);

	switch(type) {

		case MSGTYPE_MD5: {

            FileList *list = job->prepareFileList();
            msg->setJob(STREAM_MD5ONLY, list);
            LOG_U(UI_UPDATE_NODE_LOG, "MD5 info size %d", list->getCount());
        }
			break;

		default:
			delete msg;
			return false;
	}

	return send(COMP_COLLECTOR, address, msg);
}

bool Node::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}


bool Node::processMD5() {

    for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {
        Rule *rule = (Rule *)job->getContent(CONTENT_FILE, j);
        for (int i = 0; i < rule->getContentCount(CONTENT_FILE); i++) {
            FileItem *content = (FileItem *)rule->getContent(CONTENT_FILE, i);

            /*
             * false -> request file from collector, with no md5 set
             * true -> do not request file from collector, with md5 set
             */
            if (!Util::checkPath(Unit::getRootPath(COMP_NODE) ,content->getFileName(), false)) {
                content->setFlaggedToSent(false);
            }
        }
    }

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

bool Node::processRule(Rule* rule) {

    return rule->isParallel() ? processParallel(rule) : processSequential(rule);

}

bool Node::processParallel(Rule* rule) {

    int status;
    char cmd[PATH_MAX] = "";
    char *args[100];

    for (int i = 0; i < rule->getContentCount(CONTENT_EXECUTOR); i++) {
        ExecutorItem *content = (ExecutorItem *) rule->getContent(CONTENT_EXECUTOR, i);
        content->getParsed(rule, cmd);
        parseCommand(cmd, args);
        LOG_U(UI_UPDATE_NODE_LOG, "Executing %s command", cmd + strlen(Unit::getRootPath(COMP_NODE)));
        LOG_U(UI_UPDATE_NODE_EXEC_LIST, cmd);

#ifdef CYGWIN
        LOG_I("Simulating fork in Windows!!!");
#else

        pid_t pid = fork();

        if (pid == -1) {
            LOG_E("Rule Process failed in fork!!!");
            return false;

        } else if (pid == 0) {
            //child part
            execv(*args, args);
            LOG_E("ExecV failed with error : %d", errno);
            exit(EXIT_FAILURE);

        }
#endif
    }

#ifdef CYGWIN
    LOG_I("Simulating wait in Windows!!!");
#else
    while (true) {

        pid_t pid = wait(&status);
        if (pid == -1) {
            if (errno == ECHILD) {
                //no more child process
                break;
            }
        } else {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                LOG_E("Process with pid %d failed in fork!!!", pid);
                return false;
            }
        }
    }
#endif

    return true;
}

bool Node::processSequential(Rule* rule) {

    int status;
    char cmd[PATH_MAX] = "";
    char *args[100];

    for (int i = 0; i < rule->getContentCount(CONTENT_EXECUTOR); i++) {
        ExecutorItem *content = (ExecutorItem *) rule->getContent(CONTENT_EXECUTOR, i);

        content->getParsed(rule, cmd);

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
    }

    return true;
}


