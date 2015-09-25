//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "Tools.h"

Collector::Collector(uint32_t distInterfaceIndex, uint32_t clientInterfaceIndex, const std::string &rootPath) {

	mCallback = new CallBack(receiveCB, this);

	try {

		mDistributorConnector = new Connector(distInterfaceIndex, mCallback, rootPath);

	} catch (const std::runtime_error e) {

		LOG_E("Connector Init failed!!!");

		delete mCallback;

		throw std::runtime_error("Collector : Connector Init failed!!!");
	}

	if (distInterfaceIndex != clientInterfaceIndex) {

		try {

			mClientConnector = new Connector(clientInterfaceIndex, mCallback, rootPath);

		} catch (const std::runtime_error e) {

			LOG_E("Connector Init failed!!!");

			delete mDistributorConnector;
			delete mCallback;

			throw std::runtime_error("Distributor : Connector Init failed!!!");
		}

	} else {

		mClientConnector = mDistributorConnector;

	}


	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(HOST_DISTRIBUTOR), getAddress(HOST_CLIENT));

	mDistributorAddress = 0;
	mRootPath = rootPath;

	LOG_I("Instance is created!!!");

}

Collector::~Collector() {

	if (mDistributorConnector != mClientConnector) {
		delete mClientConnector;
	}

	delete mDistributorConnector;
	for (int i = 0; i < mRules.size(); i++) {
//		Rule *rule = mRules[i];
//		delete rule;
	}

	delete mCallback;

}


INTERFACES Collector::getInterfaceType(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return mDistributorConnector->getInterfaceType();
	} else {
		return mClientConnector->getInterfaceType();
	}

}

uint64_t Collector::getAddress(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return mDistributorConnector->getAddress();
	} else {
		return mClientConnector->getAddress();
	}

}


bool Collector::receiveCB(void *arg, uint64_t address, Message *msg) {

	Collector *collector = (Collector *) arg;

	switch(msg->getOwner()) {

		case HOST_DISTRIBUTOR:
			if (collector->mDistributorConnector->getInterfaceType() == Tools::getInterface(address)) {
				collector->processDistributorMsg(address, msg);
			}
			break;

		case HOST_CLIENT:
			if (collector->mClientConnector->getInterfaceType() == Tools::getInterface(address)) {
				collector->processClientMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received : %d from %s, disgarding", msg->getOwner(), Tools::getAddressStr(address).c_str());
			delete msg;
			return false;

	}
	return true;
}

bool Collector::processDistributorMsg(uint64_t address, Message *msg) {

	bool status = false;
	msg->display();

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			mDistributorAddress = address;

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"WAKEUP\" msg from distributor: %s",
					Tools::getAddressStr(address).c_str());

			status = send2DistributorMsg(address, MSGTYPE_READY);
			break;

		case MSGTYPE_CLIENT: {
				uint64_t clientaddress = msg->getVariant();
				if (clientaddress == 0) {
					LOG_W("No available client right now.");
					status = false;
					LOG_U(UI_UPDATE_COLL_LOG,
							"\"CLIENT\" msg from distributor: %s, no Available Client",
							Tools::getAddressStr(address).c_str());
					break;
				}

				mRules[clientaddress] = new Rule(getRootPath(), RULE_FILE);
				if (!mRules[clientaddress]) {
					LOG_E("Could not create a rule from path : %s", getRootPath().c_str());
					return false;
				}

				LOG_T("New Rule created from path : %s", getRootPath().c_str());

				LOG_U(UI_UPDATE_COLL_FILE_LIST, mRules[clientaddress]);
				LOG_U(UI_UPDATE_COLL_PARAM_LIST, mRules[clientaddress]);
				LOG_U(UI_UPDATE_COLL_EXEC_LIST, mRules[clientaddress]);

				LOG_U(UI_UPDATE_COLL_ATT_CLIENT_ADDRESS, clientaddress);
				LOG_U(UI_UPDATE_COLL_LOG,
						"\"CLIENT\" msg from distributor: %s, available client: %s",
						Tools::getAddressStr(address).c_str(),
						Tools::getAddressStr(clientaddress).c_str());

				status = send2ClientMsg(clientaddress, MSGTYPE_RULE);
				break;
			}
		default :
			break;
	}

	delete msg;
	return status;
}

bool Collector::processClientMsg(uint64_t address, Message *msg) {

	bool status = false;
	msg->display();

	switch(msg->getType()) {

		case MSGTYPE_MD5: {
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"MD5\" msg from client: %s with \"%d\" MD5 info",
					Tools::getAddressStr(address).c_str(), msg->mMD5List.size());

			std::map<uint64_t, Rule *>::iterator ruleItr = mRules.find(address);
			if (ruleItr == mRules.end()) {
				LOG_W("Could not find a rule for address : %lld", address);
				break;

			}

			for (int i = 0; i < msg->mMD5List.size(); i++) {
				//TODO contentler icinde ara
				for (uint16_t j = 0; j < ruleItr->second->getContentCount(RULE_FILES); j++) {
					FileContent *content = (FileContent *) ruleItr->second->getContent(RULE_FILES, j);
					//TODO 64 bit le coz
					if (memcmp(content->getMD5(), msg->mMD5List[i].mMD5, MD5_DIGEST_LENGTH) == 0) {
						//TODO bu clientte var, gonderme
						content->setFlaggedToSent(false);
					}
				}
			}

			LOG_U(UI_UPDATE_COLL_FILE_LIST, ruleItr->second);

			status = send2ClientMsg(address, MSGTYPE_BINARY);
		}
			break;

		default:
			break;

	}

	delete msg;
	return status;
}

bool Collector::send2DistributorMsg(uint64_t address, uint8_t type) {

	Message *msg = new Message(getRootPath());
	msg->setOwner(HOST_COLLECTOR);
	msg->setTarget(HOST_DISTRIBUTOR);
	msg->setType(type);

	switch(type) {

		case MSGTYPE_READY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"READY\" msg sent to distributor: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_CLIENT:
			msg->setPriority(PRIORITY_2);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"CLIENT\" msg sent to distributor: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_TIME:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"TIME\" msg sent to distributor: %s",
					Tools::getAddressStr(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return mDistributorConnector->send(address, msg);

}

bool Collector::send2ClientMsg(uint64_t address, uint8_t type) {

	Message *msg = new Message(getRootPath());
	msg->setOwner(HOST_COLLECTOR);
	msg->setTarget(HOST_CLIENT);
	msg->setType(type);

	std::map<uint64_t, Rule*>::iterator ruleItr = mRules.find(address);
	if (ruleItr == mRules.end()) {
		LOG_W("Could not find a rule for address : %lld", address);
		delete msg;
		return false;

	}

	switch(type) {

		case MSGTYPE_RULE:
			msg->setPriority(PRIORITY_3);
			msg->setRule(STREAM_RULE, ruleItr->second);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"RULE\" msg sent to client: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_BINARY:
			msg->setPriority(PRIORITY_3);
			msg->setRule(STREAM_BINARY, ruleItr->second);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"BINARY\" msg sent to client: %s with \"%d\" file binary",
					Tools::getAddressStr(address).c_str(), ruleItr->second->getFlaggedFileCount());
			break;

		default:
			delete msg;
			return false;

	}

	return mClientConnector->send(address, msg);

}

bool Collector::processRule(const std::string &path) {

	if (path.compare(getRootPath()) != 0) {
		setRootPath(path);
	}

	return send2DistributorMsg(mDistributorAddress, MSGTYPE_CLIENT);
}

bool Collector::processRule() {

	return processRule(getRootPath());

}

bool Collector::syncTime() {

	return send2DistributorMsg(mDistributorAddress, MSGTYPE_TIME);

}

std::string Collector::getRootPath() {

	return mRootPath;

}

void Collector::setRootPath(const std::string &path) {

	mRootPath = path;
//	mDistributorConnector->setRootPath(path);
//	mClientConnector->setRootPath(path);

}

void Collector::display() {

	//mRule->display();

}


bool Collector::reset() {

	mRules.clear();
	return true;

}
