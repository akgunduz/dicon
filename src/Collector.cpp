//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "Tools.h"
#include "NetAddress.h"

Collector::Collector(int distributorIndex, int nodeIndex, const char *rootPath) :
        Component(distributorIndex, nodeIndex, rootPath){

	this->distributorIndex = 0;
	this->nodeIndex = 1;
    printf("Collector %p\n", this);

	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(HOST_DISTRIBUTOR), getAddress(HOST_CLIENT));

	distributorAddress = 0;

	LOG_I("Instance is created!!!");

}

Collector::~Collector() {

	for (int i = 0; i < rules.size(); i++) {
//		Rule *rule = mRules[i];
//		delete rule;
	}

}


INTERFACES Collector::getInterfaceType(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return connectors[distributorIndex]->getInterfaceType();
	} else {
		return connectors[nodeIndex]->getInterfaceType();
	}

}

long Collector::getAddress(HOST host) {

	if (host == HOST_DISTRIBUTOR) {
		return connectors[distributorIndex]->getAddress();
	} else {
		return connectors[nodeIndex]->getAddress();
	}

}

bool Collector::onReceive(long address, Message *msg) {

	switch(msg->getOwner()) {

		case HOST_DISTRIBUTOR:
			if (connectors[distributorIndex]->getInterfaceType() == Address::getInterface(address)) {
				processDistributorMsg(address, msg);
			}
			break;

		case HOST_CLIENT:
			if (connectors[nodeIndex]->getInterfaceType() == Address::getInterface(address)) {
				processClientMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received : %d from %s, disgarding", msg->getType(), Address::getString(address).c_str());
			delete msg;
			return false;

	}
	return true;
}

bool Collector::processDistributorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			distributorAddress = address;

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"WAKEUP\" msg from distributor: %s", Address::getString(address).c_str());

			status = send2DistributorMsg(address, MSGTYPE_READY);
			break;

		case MSGTYPE_CLIENT: {

            long clientAddress = msg->getVariant(0);

			if (clientAddress == 0) {
				LOG_W("No available client right now.");
				status = false;
				LOG_U(UI_UPDATE_COLL_LOG,
						"\"CLIENT\" msg from distributor: %s, no Available Client", Address::getString(address).c_str());
				break;
			}

			rules[clientAddress] = new Rule(getRootPath(), RULE_FILE);
			if (!rules[clientAddress]) {
				LOG_E("Could not create a rule from path : %s", getRootPath());
				return false;
			}

			LOG_T("New Rule created from path : %s", getRootPath());

			LOG_U(UI_UPDATE_COLL_FILE_LIST, rules[clientAddress]);
			LOG_U(UI_UPDATE_COLL_PARAM_LIST, rules[clientAddress]);
			LOG_U(UI_UPDATE_COLL_EXEC_LIST, rules[clientAddress]);

			LOG_U(UI_UPDATE_COLL_ATT_CLIENT_ADDRESS, clientAddress);
			LOG_U(UI_UPDATE_COLL_LOG,
                  "\"CLIENT\" msg from distributor: %s, available client: %s",
				  Address::getString(address).c_str(), Address::getString(clientAddress).c_str());

			status = send2ClientMsg(clientAddress, MSGTYPE_RULE);
			break;
			}
		default :
			break;
	}

	delete msg;
	return status;
}

bool Collector::processClientMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_MD5: {
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"MD5\" msg from client: %s with \"%d\" MD5 info",
				  Address::getString(address).c_str(), msg->mMD5List.size());

			std::map<long, Rule *>::iterator ruleItr = rules.find(address);
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

bool Collector::send2DistributorMsg(long address, int type) {

	Message *msg = new Message(HOST_COLLECTOR, type, getRootPath());

	switch(type) {

		case MSGTYPE_READY:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"READY\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_CLIENT:
			msg->setPriority(PRIORITY_2);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"CLIENT\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_TIME:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"TIME\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[distributorIndex]->send(address, msg);

}

bool Collector::send2ClientMsg(long address, int type) {

	Message *msg = new Message(HOST_COLLECTOR, type, getRootPath());

	std::map<long, Rule*>::iterator ruleItr = rules.find(address);
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
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_BINARY:
			msg->setPriority(PRIORITY_3);
			msg->setRule(STREAM_BINARY, ruleItr->second);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"BINARY\" msg sent to client: %s with \"%d\" file binary",
				  Address::getString(address).c_str(), ruleItr->second->getFlaggedFileCount());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[nodeIndex]->send(address, msg);

}

bool Collector::processRule(const std::string &path) {

//	if (path.compare(getRootPath()) != 0) {
//		setRootPath(path);
//	}

	return send2DistributorMsg(distributorAddress, MSGTYPE_CLIENT);
}

bool Collector::processRule() {

	return processRule(getRootPath());

}

bool Collector::syncTime() {

	return send2DistributorMsg(distributorAddress, MSGTYPE_TIME);

}

void Collector::display() {

	//mRule->display();

}


bool Collector::reset() {

	rules.clear();
	return true;

}
