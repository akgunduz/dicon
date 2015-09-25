//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <sys/wait.h>
#include "Client.h"
#include "Tools.h"

Client::Client(uint32_t distInterfaceIndex, uint32_t collInterfaceIndex, const std::string &rootPath) {

	mCallback = new CallBack(receiveCB, this);

	try {

		mDistributorConnector = new Connector(distInterfaceIndex, mCallback, rootPath);

	} catch (const std::runtime_error e) {

		LOG_E("Connector Init failed!!!");

		delete mCallback;

		throw std::runtime_error("Collector : Connector Init failed!!!");
	}

	if (distInterfaceIndex != collInterfaceIndex) {

		try {

			mCollectorConnector = new Connector(collInterfaceIndex, mCallback, rootPath);

		} catch (const std::runtime_error e) {

			LOG_E("Connector Init failed!!!");

			delete mDistributorConnector;
			delete mCallback;

			throw std::runtime_error("Distributor : Connector Init failed!!!");
		}

	} else {

		mCollectorConnector = mDistributorConnector;

	}

	LOG_U(UI_UPDATE_CLIENT_ADDRESS, getAddress(HOST_DISTRIBUTOR), getAddress(HOST_COLLECTOR));
	LOG_U(UI_UPDATE_CLIENT_STATE, IDLE);

	mDistributorAddress = 0;
	mState = IDLE;
	mRootPath = rootPath;

	LOG_I("Instance is created!!!");

}

Client::~Client() {

	if (mDistributorConnector != mCollectorConnector) {
		delete mCollectorConnector;
	}

	delete mDistributorConnector;

	delete mCallback;

}

INTERFACES Client::getInterfaceType(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return mDistributorConnector->getInterfaceType();
	} else {
		return mCollectorConnector->getInterfaceType();
	}

}

uint64_t Client::getAddress(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return mDistributorConnector->getAddress();
	} else {
		return mCollectorConnector->getAddress();
	}

}

bool Client::receiveCB(void *arg, uint64_t address, Message *msg) {

	Client *client = (Client *) arg;

	switch(msg->getOwner()) {

		case HOST_DISTRIBUTOR:
			if (client->mDistributorConnector->getInterfaceType() == Tools::getInterface(address)) {
				client->processDistributorMsg(address, msg);
			}
			break;

		case HOST_COLLECTOR:
			if (client->mCollectorConnector->getInterfaceType() == Tools::getInterface(address)) {
				client->processCollectorMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received with owner ID: %d from %s, disgarding!!!",
					msg->getOwner(), Tools::getAddressStr(address).c_str());
			delete msg;
			break;
	}
	return true;
}

bool Client::processCollectorMsg(uint64_t address, Message *msg) {

	bool status = false;
	msg->display();

	switch(msg->getType()) {

		case MSGTYPE_RULE:

			LOG_U(UI_UPDATE_CLIENT_STATE, BUSY);
			status = send2DistributorMsg(mDistributorAddress, MSGTYPE_BUSY);

			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"RULE\" msg from collector: %s",
					Tools::getAddressStr(address).c_str());

			mRule = msg->mRule;

			if (!processMD5()) {
				LOG_E("Processing MD5 failed!!!");
				break;
			}

			LOG_U(UI_UPDATE_CLIENT_FILE_LIST, mRule);
			LOG_U(UI_UPDATE_CLIENT_PARAM_LIST, mRule);
			LOG_U(UI_UPDATE_CLIENT_EXEC_LIST, mRule);

			status &= send2CollectorMsg(address, MSGTYPE_MD5);
			break;

		case MSGTYPE_BINARY:

			mRule = msg->mRule;
			if (mRule == nullptr) {
				mRule = new Rule(mRootPath, RULE_FILE);
			}

			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"BINARY\" msg from collector: %s with \"%d\" file binary",
					Tools::getAddressStr(address).c_str(), msg->getReceivedBinaryCount());

			LOG_U(UI_UPDATE_CLIENT_FILE_LIST, mRule);

			processRule();

			LOG_U(UI_UPDATE_CLIENT_STATE, IDLE);
			status = send2DistributorMsg(mDistributorAddress, MSGTYPE_READY);
			break;

		default :
			break;
	}

	delete msg;
	return status;
}

bool Client::processDistributorMsg(uint64_t address, Message *msg) {

	bool status = false;
	msg->display();

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			mDistributorAddress = address;

			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"WAKEUP\" msg from distributor: %s",
					Tools::getAddressStr(address).c_str());

			status = send2DistributorMsg(address, MSGTYPE_ALIVE);
			break;

		default :
			break;
	}

	delete msg;
	return status;
}

bool Client::send2DistributorMsg(uint64_t address, uint8_t type) {

	Message *msg = new Message(mRootPath);
	msg->setOwner(HOST_CLIENT);
	msg->setTarget(HOST_DISTRIBUTOR);
	msg->setType(type);

	switch(type) {

		case MSGTYPE_READY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"READY\" msg sent to distributor: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_ALIVE:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"ALIVE\" msg sent to distributor: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_BUSY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"BUSY\" msg sent to distributor: %s",
					Tools::getAddressStr(address).c_str());
			break;

		default:
			delete msg;
			return false;
	}

	return mDistributorConnector->send(address, msg);

}

bool Client::send2CollectorMsg(uint64_t address, uint8_t type) {

	Message *msg = new Message(mRootPath);
	msg->setOwner(HOST_CLIENT);
	msg->setTarget(HOST_COLLECTOR);
	msg->setType(type);

	switch(type) {

		case MSGTYPE_MD5:
			msg->setPriority(PRIORITY_2);
			msg->setRule(STREAM_MD5ONLY, mRule);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"\"MD5\" msg sent to collector: %s with \"%d\" MD5 info",
					Tools::getAddressStr(address).c_str(), mRule->getFlaggedFileCount());
			break;

		default:
			delete msg;
			return false;
	}

	return mCollectorConnector->send(address, msg);
}

bool Client::processMD5() {

	for (uint16_t i = 0; i < mRule->getContentCount(RULE_FILES); i++) {

		FileContent *content = (FileContent *)mRule->getContent(RULE_FILES, i);

		std::string abspath = mRootPath + content->getPath();

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

	if (mRule->getRunType() == RUN_SEQUENTIAL) {

		for (int i = 0; i < mRule->getContentCount(RULE_EXECUTORS); i++) {
			ExecutorContent *content = (ExecutorContent *) mRule->getContent(RULE_EXECUTORS, i);
			std::string cmd = content->getParsed(mRule);
			LOG_U(UI_UPDATE_CLIENT_LOG,
					"Executing %s command", cmd.c_str());

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

		}

		return true;
	}

	//parallel process
	for (int i = 0; i < mRule->getContentCount(RULE_EXECUTORS); i++) {
		ExecutorContent *content = (ExecutorContent *)mRule->getContent(RULE_EXECUTORS, i);
		std::string cmd = content->getParsed(mRule);
		LOG_U(UI_UPDATE_CLIENT_LOG,
				"Executing %s command", cmd.c_str());

		pid_t pid = fork();

		if (pid == -1) {
			LOG_E("Rule Process failed in fork!!!");
			return false;

		} else if (pid == 0) {
			//child part
			processExecutor(cmd);

		}

	}

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

	return true;
}

std::string Client::getRootPath() {

	return mRootPath;

}


