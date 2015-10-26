//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "Util.h"
#include "NetAddress.h"

Collector::Collector(int distributorIndex, int nodeIndex, const char *rootPath) :
        Component(Unit(HOST_COLLECTOR, Util::getID()), generateIndex(distributorIndex, 0xFFFF, nodeIndex), rootPath){

	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(HOST_DISTRIBUTOR), getAddress(HOST_NODE));

	distributorAddress = 0;

	LOG_I("Instance is created!!!");

}

Collector::~Collector() {

	for (std::map<long , Job*>::iterator i = jobs.begin(); i != jobs.end(); i++) {
        delete i->second;
	}
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

		case MSGTYPE_NODE: {

            long clientAddress = msg->getVariant(0);
			short clientID = (short) msg->getVariant(1);

			if (clientAddress == 0) {
				LOG_W("No available client right now.");
				status = false;
				LOG_U(UI_UPDATE_COLL_LOG,
						"\"CLIENT\" msg from distributor: %s, no Available Node", Address::getString(address).c_str());
				break;
			}

			rules[clientAddress] = new Rule(Unit(HOST_COLLECTOR), Unit(HOST_NODE, clientID), getRootPath());

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

bool Collector::processCollectorMsg(long address, Message *msg) {
    return false;
}

bool Collector::processNodeMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_MD5: {

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"MD5\" msg from client: %s with \"%d\" MD5 info",
				  Address::getString(address).c_str(), msg->md5List.size());

			std::map<long, Rule *>::iterator ruleItr = rules.find(address);
			if (ruleItr == rules.end()) {
				LOG_W("Could not find a rule for address : %lld", address);
				break;

			}

			for (int i = 0; i < msg->md5List.size(); i++) {
				//TODO contentler icinde ara
				for (uint16_t j = 0; j < ruleItr->second->getContentCount(CONTENT_FILE); j++) {
					FileContent *content = (FileContent *) ruleItr->second->getContent(CONTENT_FILE, j);
					//TODO 64 bit le coz
					if (memcmp(content->getMD5(), msg->md5List[i].md5, MD5_DIGEST_LENGTH) == 0) {
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

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"READY\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_NODE:

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"CLIENT\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_TIME:

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"TIME\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[HOST_DISTRIBUTOR]->send(address, msg);

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

			msg->setRule(STREAM_RULE, ruleItr->second);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"RULE\" msg sent to client: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_BINARY:

			msg->setRule(STREAM_BINARY, ruleItr->second);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"BINARY\" msg sent to client: %s with \"%d\" file binary",
				  Address::getString(address).c_str(), ruleItr->second->getFlaggedFileCount());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[HOST_NODE]->send(address, msg);

}

bool Collector::processRule(const std::string &path) {

//	if (path.compare(getRootPath()) != 0) {
//		setRootPath(path);
//	}

	return send2DistributorMsg(distributorAddress, MSGTYPE_NODE);
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