//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "Tools.h"
#include "NetAddress.h"

Collector::Collector(uint32_t distInterfaceIndex, uint32_t clientInterfaceIndex, const std::string &rootPath) {

	callback = new InterfaceCallback(receiveCB, this);

	try {

		distributorConnector = new Connector(distInterfaceIndex, callback, rootPath);

	} catch (const std::runtime_error e) {

		LOG_E("Connector Init failed!!!");

		delete callback;

		throw std::runtime_error("Collector : Connector Init failed!!!");
	}

	if (distInterfaceIndex != clientInterfaceIndex) {

		try {

			clientConnector = new Connector(clientInterfaceIndex, callback, rootPath);

		} catch (const std::runtime_error e) {

			LOG_E("Connector Init failed!!!");

			delete distributorConnector;
			delete callback;

			throw std::runtime_error("Distributor : Connector Init failed!!!");
		}

	} else {

		clientConnector = distributorConnector;

	}


	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(HOST_DISTRIBUTOR)->getAddress(), getAddress(HOST_CLIENT)->getAddress());

	distributorAddress = 0;
	this->rootPath = rootPath;

	LOG_I("Instance is created!!!");

}

Collector::~Collector() {

	if (distributorConnector != clientConnector) {
		delete clientConnector;
	}

	delete distributorConnector;
	for (int i = 0; i < rules.size(); i++) {
//		Rule *rule = mRules[i];
//		delete rule;
	}

	delete callback;

}


INTERFACES Collector::getInterfaceType(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return distributorConnector->getInterfaceType();
	} else {
		return clientConnector->getInterfaceType();
	}

}

Address* Collector::getAddress(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return distributorConnector->getAddress();
	} else {
		return clientConnector->getAddress();
	}

}


bool Collector::receiveCB(void *arg, Address* address, Message *msg) {

	Collector *collector = (Collector *) arg;

	switch(msg->getOwner()) {

		case HOST_DISTRIBUTOR:
			if (collector->distributorConnector->getInterfaceType() == address->getInterface()) {
				collector->processDistributorMsg(address, msg);
			}
			break;

		case HOST_CLIENT:
			if (collector->clientConnector->getInterfaceType() == address->getInterface()) {
				collector->processClientMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received : %d from %s, disgarding", msg->getOwner(), address->getString().c_str());
			delete msg;
			return false;

	}
	return true;
}

bool Collector::processDistributorMsg(Address* address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			distributorAddress = address;

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"WAKEUP\" msg from distributor: %s", address->getString().c_str());

			status = send2DistributorMsg(address, MSGTYPE_READY);
			break;

		case MSGTYPE_CLIENT: {

            Address *clientAddress = new NetAddress(msg->getVariant(0));

			if (clientAddress->getAddress() == 0) {
				LOG_W("No available client right now.");
				status = false;
				LOG_U(UI_UPDATE_COLL_LOG,
						"\"CLIENT\" msg from distributor: %s, no Available Client", address->getString().c_str());
				break;
			}

			rules[clientAddress->getAddress()] = new Rule(getRootPath(), RULE_FILE);
			if (!rules[clientAddress->getAddress()]) {
				LOG_E("Could not create a rule from path : %s", getRootPath().c_str());
				return false;
			}

			LOG_T("New Rule created from path : %s", getRootPath().c_str());

			LOG_U(UI_UPDATE_COLL_FILE_LIST, rules[clientAddress->getAddress()]);
			LOG_U(UI_UPDATE_COLL_PARAM_LIST, rules[clientAddress->getAddress()]);
			LOG_U(UI_UPDATE_COLL_EXEC_LIST, rules[clientAddress->getAddress()]);

			LOG_U(UI_UPDATE_COLL_ATT_CLIENT_ADDRESS, clientAddress->getAddress());
			LOG_U(UI_UPDATE_COLL_LOG,
                  "\"CLIENT\" msg from distributor: %s, available client: %s",
                  address->getString().c_str(), clientAddress->getString().c_str());

			status = send2ClientMsg(clientAddress, MSGTYPE_RULE);
			break;
			}
		default :
			break;
	}

	delete msg;
	return status;
}

bool Collector::processClientMsg(Address* address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_MD5: {
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"MD5\" msg from client: %s with \"%d\" MD5 info",
                  address->getString().c_str(), msg->mMD5List.size());

			std::map<long, Rule *>::iterator ruleItr = rules.find(address->getAddress());
			if (ruleItr == rules.end()) {
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

bool Collector::send2DistributorMsg(Address* address, int type) {

	Message *msg = new Message(HOST_COLLECTOR, type, rootPath);

	switch(type) {

		case MSGTYPE_READY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"READY\" msg sent to distributor: %s",
                  address->getString().c_str());
			break;

		case MSGTYPE_CLIENT:
			msg->setPriority(PRIORITY_2);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"CLIENT\" msg sent to distributor: %s",
                  address->getString().c_str());
			break;

		case MSGTYPE_TIME:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"TIME\" msg sent to distributor: %s",
                  address->getString().c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return distributorConnector->send(address, msg);

}

bool Collector::send2ClientMsg(Address* address, int type) {

	Message *msg = new Message(HOST_COLLECTOR, type, rootPath);

	std::map<long, Rule*>::iterator ruleItr = rules.find(address->getAddress());
	if (ruleItr == rules.end()) {
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
                  address->getString().c_str());
			break;

		case MSGTYPE_BINARY:
			msg->setPriority(PRIORITY_3);
			msg->setRule(STREAM_BINARY, ruleItr->second);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"BINARY\" msg sent to client: %s with \"%d\" file binary",
                  address->getString().c_str(), ruleItr->second->getFlaggedFileCount());
			break;

		default:
			delete msg;
			return false;

	}

	return clientConnector->send(address, msg);

}

bool Collector::processRule(const std::string &path) {

	if (path.compare(getRootPath()) != 0) {
		setRootPath(path);
	}

	return send2DistributorMsg(distributorAddress, MSGTYPE_CLIENT);
}

bool Collector::processRule() {

	return processRule(getRootPath());

}

bool Collector::syncTime() {

	return send2DistributorMsg(distributorAddress, MSGTYPE_TIME);

}

std::string Collector::getRootPath() {

	return rootPath;

}

void Collector::setRootPath(const std::string &path) {

	rootPath = path;
//	mDistributorConnector->setRootPath(path);
//	mClientConnector->setRootPath(path);

}

void Collector::display() {

	//mRule->display();

}


bool Collector::reset() {

	rules.clear();
	return true;

}
