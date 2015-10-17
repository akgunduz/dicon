//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"

Distributor::Distributor(int collectorIndex,
		int nodeIndex, const char *rootPath, double backupRate) :
            Component(Unit(HOST_DISTRIBUTOR, Util::getID()),
					  generateIndex(0xFFFF, collectorIndex, nodeIndex), rootPath){

	LOG_U(UI_UPDATE_DIST_ADDRESS, getAddress(HOST_COLLECTOR), getAddress(HOST_NODE));

	clientManager = new ClientManager(connectors[nodeIndex],
			timeoutCallback, sendWakeupMessage, backupRate);
	clientManager->initClientChecker();

};

Distributor::~Distributor() {

	delete clientManager;
}

bool Distributor::processDistributorMsg(long address, Message *msg) {
	return false;
}

bool Distributor::processCollectorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from collector: %s", Address::getString(address).c_str());
			break;

		case MSGTYPE_CLIENT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"CLIENT\" msg from collector: %s", Address::getString(address).c_str());

			status = send2CollectorMsg(address, MSGTYPE_CLIENT);
			break;

		case MSGTYPE_TIME:
			collStartTime.start();
			clientManager->resetDiffTimes();
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIME\" msg from collector: %s", Address::getString(address).c_str());
			break;

		default:
			break;
	}

	delete msg;
	return status;

}

bool Distributor::processClientMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from client: %s", Address::getString(address).c_str());

			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, IDLE);

			clientManager->setClientIdle(address, msg->getOwner().getID(), collStartTime.stop());

			if (collectorWaitingList.size() > 0) {

				long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
                      "Processing a collector from the waiting list: %s",
					  Address::getString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_CLIENT);

			} else {
				status = true;
			}

			break;

		case MSGTYPE_ALIVE:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"ALIVE\" msg from client: %s", Address::getString(address).c_str());

			if (!clientManager->setClientValidate(address, msg->getOwner().getID())
					&& collectorWaitingList.size() > 0) {

                long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
						"Processing a collector from the waiting list: %s", Address::getString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_CLIENT);
			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, IDLE);

			break;

		case MSGTYPE_BUSY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"BUSY\" msg from client: %s", Address::getString(address).c_str());

			clientManager->setClientBusy(address);
			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, BUSY);


			status = true;
			break;

		case MSGTYPE_TIMEOUT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIMEOUT\" msg from client: %s", Address::getString(address).c_str());

			clientManager->setClientRemove(address);
			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, REMOVE);

			status = send2CollectorMsg(msg->getVariant(0), MSGTYPE_CLIENT);

			break;

		default :
			break;
	}

	delete msg;
	return status;

}

bool Distributor::send2ClientMsg(long address, uint8_t type) {

	Message *msg = new Message(HOST_DISTRIBUTOR, type, getRootPath());

	switch(type) {

		case MSGTYPE_WAKEUP:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to client: %s", Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[HOST_NODE]->send(address, msg);

}

bool Distributor::send2CollectorMsg(long address, uint8_t type) {

	Message *msg = new Message(HOST_DISTRIBUTOR, type, getRootPath());

	switch(type) {

		case MSGTYPE_WAKEUP:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to collector: %s", Address::getString(address).c_str());
			break;

		case MSGTYPE_CLIENT: {
				msg->setPriority(PRIORITY_2);
				ClientMap* client = clientManager->getIdleClient(address);

				if (client != nullptr) {

					LOG_U(UI_UPDATE_DIST_CLIENT_LIST, client->address, PREBUSY);
					LOG_U(UI_UPDATE_DIST_LOG,
							"\"CLIENT\" msg sent to collector: %s with available client: %s",
						  Address::getString(address).c_str(),
						  Address::getString(client->address).c_str());

                    msg->setVariant(0, client->address);
                    msg->setVariant(1, client->id);

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, client->address);

				} else {
					collectorWaitingList.push_back(address);

					LOG_U(UI_UPDATE_DIST_LOG,
							"\"CLIENT\" msg sent to collector: %s with no available client",
						  Address::getString(address).c_str());

                    msg->setVariant(0, 0);

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
				}

			}
			break;

		default:
			delete msg;
			return false;
	}

	return connectors[HOST_COLLECTOR]->send(address, msg);

}

bool Distributor::sendWakeupMessagesAll() {

	sendWakeupMessage(connectors[HOST_NODE]);
	if (connectors[HOST_COLLECTOR] != connectors[HOST_NODE]) {
		sendWakeupMessage(connectors[HOST_COLLECTOR]);
	}
	return true;
}

bool Distributor::reset() {

	clientManager->clear();
	collectorWaitingList.clear();
	collStartTime.reset();
	return true;

}

bool Distributor::timeoutCallback(Connector *connector,
								  long address, long collectorAddress) {

	Message *msg = new Message(HOST_NODE, MSGTYPE_TIMEOUT, connector->getRootPath());
	msg->setVariant(0, collectorAddress);
	connector->getInterface()->
			push(MESSAGE_RECEIVE, address, msg);

	return true;
}

bool Distributor::sendWakeupMessage(Connector *connector) {

	std::vector<long> list = connector->getAddressList();

	for (int i = 0; i < list.size(); i++) {

		Message *msg = new Message(HOST_DISTRIBUTOR, MSGTYPE_WAKEUP, connector->getRootPath());
		msg->setPriority(PRIORITY_1);
		//LOG_I("\"WAKEUP\" messages sent to : %s", Address::getString(list[i]).c_str());
		connector->send(list[i], msg);

	}

	LOG_U(UI_UPDATE_DIST_LOG,
			"\"WAKEUP\" messages sent to network");

	return true;
}
