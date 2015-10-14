//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Client.h"

Client::Client(int distributorIndex, int collectorIndex, const char *rootPath) :
        Component(distributorIndex, collectorIndex, rootPath) {

	this->distributorIndex = 0;
	this->collectorIndex = 1;

	LOG_U(UI_UPDATE_CLIENT_ADDRESS, getAddress(HOST_DISTRIBUTOR), getAddress(HOST_COLLECTOR));
	LOG_U(UI_UPDATE_CLIENT_STATE, IDLE);

	distributorAddress = 0;

	LOG_I("Instance is created!!!");

}

Client::~Client() {

}

INTERFACES Client::getInterfaceType(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return connectors[distributorIndex]->getInterfaceType();
	} else {
		return connectors[collectorIndex]->getInterfaceType();
	}

}

long Client::getAddress(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return connectors[distributorIndex]->getAddress();
	} else {
		return connectors[collectorIndex]->getAddress();
	}

}

bool Client::onReceive(long address, Message *msg) {

	switch(msg->getOwner()) {

		case HOST_DISTRIBUTOR:
			if (connectors[distributorIndex]->getInterfaceType() == Address::getInterface(address)) {
				processDistributorMsg(address, msg);
			}
			break;

		case HOST_COLLECTOR:
			if (connectors[collectorIndex]->getInterfaceType() == Address::getInterface(address)) {
				processCollectorMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received with owner ID: %d from %s, disgarding!!!",
					msg->getOwner(), Address::getString(address).c_str());
			delete msg;
			break;
	}
	return true;
}

bool Client::processCollectorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_RULE:

			LOG_U(UI_UPDATE_CLIENT_STATE, BUSY);
			status = send2DistributorMsg(distributorAddress, MSGTYPE_BUSY);

			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"RULE\" msg from collector: %s", Address::getString(address).c_str());

			rule = msg->mRule;

			if (!processMD5()) {
				LOG_E("Processing MD5 failed!!!");
				break;
			}

			LOG_U(UI_UPDATE_CLIENT_FILE_LIST, rule);
			LOG_U(UI_UPDATE_CLIENT_PARAM_LIST, rule);
			LOG_U(UI_UPDATE_CLIENT_EXEC_LIST, rule);

			status &= send2CollectorMsg(address, MSGTYPE_MD5);
			break;

		case MSGTYPE_BINARY:

			rule = msg->mRule;
			if (rule == nullptr) {
				rule = new Rule(getRootPath(), RULE_FILE);
			}

			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"BINARY\" msg from collector: %s with \"%d\" file binary",
				  Address::getString(address).c_str(), 0/*msg->getReceivedBinaryCount()*/);

			LOG_U(UI_UPDATE_CLIENT_FILE_LIST, rule);

			processRule();

			LOG_U(UI_UPDATE_CLIENT_STATE, IDLE);
			status = send2DistributorMsg(distributorAddress, MSGTYPE_READY);
			break;

		default :
			break;
	}

	delete msg;
	return status;
}

bool Client::processDistributorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			distributorAddress = address;

			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"WAKEUP\" msg from distributor: %s", Address::getString(address).c_str());

			status = send2DistributorMsg(address, MSGTYPE_ALIVE);
			break;

		default :
			break;
	}

	delete msg;
	return status;
}

bool Client::send2DistributorMsg(long address, uint8_t type) {

	Message *msg = new Message(HOST_CLIENT, type, getRootPath());

	switch(type) {

		case MSGTYPE_READY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"READY\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_ALIVE:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"ALIVE\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_BUSY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"BUSY\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;
	}

	return connectors[distributorIndex]->send(address, msg);

}

bool Client::send2CollectorMsg(long address, uint8_t type) {

	Message *msg = new Message(HOST_CLIENT, type, getRootPath());

	switch(type) {

		case MSGTYPE_MD5:
			msg->setPriority(PRIORITY_2);
			msg->setRule(STREAM_MD5ONLY, rule);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"MD5\" msg sent to collector: %s with \"%d\" MD5 info",
				  Address::getString(address).c_str(), rule->getFlaggedFileCount());
			break;

		default:
			delete msg;
			return false;
	}

	return connectors[collectorIndex]->send(address, msg);
}

bool Client::processMD5() {

	for (uint16_t i = 0; i < rule->getContentCount(RULE_FILES); i++) {

		FileContent *content = (FileContent *)rule->getContent(RULE_FILES, i);

		std::string abspath = getRootPath() + content->getPath();

		//Burada ters lojik var,
		//	false -> collectordan dosyayi iste, md5 i set ETMEYEREK
		//	true -> collectordan dosyayi isteme, md5 i set EDEREK
		if (access(abspath.c_str(), F_OK ) == -1) {
                        //printf("error %d\n", errno);
			content->setFlaggedToSent(false);
		}

	}

	return true;

}

void Client::processExecutor(std::string cmd) {

	size_t pos = 0, newpos = 0;
	char *cmdargs[100];
	int j;
	std::string subs;

	char *buffer = (char *) malloc(100 * 255);
	if (buffer == nullptr) {
		LOG_E("Malloc failed");
		exit(EXIT_FAILURE);
	}

	for (j = 0; j < 100; j++) {
		cmdargs[j] = buffer + (j * 255);
	}

	j = 0;

	do {

		newpos = cmd.find(' ', pos);
		if (newpos == std::string::npos) {
			subs = cmd.substr(pos, cmd.length() - pos);
			strcpy(cmdargs[j], subs.c_str());
			cmdargs[j+1] = nullptr;
			break;
		}
		subs = cmd.substr(pos, newpos - pos);
		strcpy(cmdargs[j], subs.c_str());
		pos = newpos + 1;
		j++;

	} while(1);
	LOG_I("ExecV run with cmd : %s", cmdargs[0]);
	execv(cmdargs[0], cmdargs);
	LOG_E("ExecV failed with error : %d", errno);
	exit(EXIT_FAILURE);
}

bool Client::processRule() {

	int status;

	if (rule->getRunType() == RUN_SEQUENTIAL) {

		for (int i = 0; i < rule->getContentCount(RULE_EXECUTORS); i++) {
			ExecutorContent *content = (ExecutorContent *) rule->getContent(RULE_EXECUTORS, i);
			std::string cmd = content->getParsed(rule);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"Executing %s command", cmd.c_str());
#ifdef CYGWIN
			LOG_I("Simulating fork in Windows!!!");
#else
			pid_t pid = fork();

			if (pid == -1) {
				LOG_E("Rule Process failed in fork!!!");
				return false;

			} else if (pid > 0) {
				//parent part
				waitpid(pid, &status, 0);

			} else {
				//child part
				processExecutor(cmd);
			}
#endif
		}

		return true;
	}

	//parallel process
	for (int i = 0; i < rule->getContentCount(RULE_EXECUTORS); i++) {
		ExecutorContent *content = (ExecutorContent *)rule->getContent(RULE_EXECUTORS, i);
		std::string cmd = content->getParsed(rule);
		LOG_U(UI_UPDATE_CLIENT_LOG,
				"Executing %s command", cmd.c_str());

#ifdef CYGWIN
		LOG_I("Simulating fork in Windows!!!");
#else
		pid_t pid = fork();

		if (pid == -1) {
			LOG_E("Rule Process failed in fork!!!");
			return false;

		} else if (pid == 0) {
			//child part
			processExecutor(cmd);

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


