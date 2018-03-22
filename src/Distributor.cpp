//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "ArchTypes.h"

Distributor::Distributor(const char *rootPath, double backupRate) :
            Component(Unit(COMP_DISTRIBUTOR), rootPath){

	nodeManager = new NodeManager(this, onTimeOut, onWakeup, backupRate);

    LOG_U(UI_UPDATE_DIST_ADDRESS, getAddress(COMP_COLLECTOR), getAddress(COMP_NODE));
};

Distributor::~Distributor() {

	delete nodeManager;
}

bool Distributor::processDistributorMsg(long address, Message *msg) {
	return false;
}

bool Distributor::processCollectorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_ALIVE:
			LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"ALIVE\" msg from collector: %s", Address::getString(address).c_str());
			break;

		case MSGTYPE_NODE:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"NODE\" msg from collector: %s", Address::getString(address).c_str());

			status = send2CollectorMsg(address, MSGTYPE_NODE);
			break;

		case MSGTYPE_TIME:
			collStartTime.start();
			nodeManager->resetTimes();
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIME\" msg from collector: %s", Address::getString(address).c_str());
			break;

		default:
			break;
	}

	delete msg;
	return status;

}

bool Distributor::processNodeMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from node: %s with ID : %s",
                  Address::getString(address).c_str(), ArchTypes::getDir(msg->getOwner().getArch()));

			nodeManager->setIdle(address, msg->getOwner().getArch(), collStartTime.stop());

			if (collectorWaitingList.size() > 0) {

				long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
                      "Processing a collector from the waiting list: %s",
					  Address::getString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_NODE);

			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

			break;

		case MSGTYPE_ALIVE:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"ALIVE\" msg from node: %s with ID : %s",
                  Address::getString(address).c_str(), ArchTypes::getDir(msg->getOwner().getArch()));

			if (!nodeManager->validate(address, msg->getOwner().getArch())
					&& collectorWaitingList.size() > 0) {

                long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
						"Processing a collector from the waiting list: %s", Address::getString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_NODE);
			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

			break;

		case MSGTYPE_BUSY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"BUSY\" msg from node: %s", Address::getString(address).c_str());

			nodeManager->setBusy(address);
			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, BUSY);


			status = true;
			break;

		case MSGTYPE_TIMEOUT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIMEOUT\" msg from node: %s", Address::getString(address).c_str());

			nodeManager->remove(address);
			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, REMOVE);

			status = send2CollectorMsg(msg->getVariant(0), MSGTYPE_NODE);

			break;

		default :
			break;
	}

	delete msg;
	return status;

}

bool Distributor::send2NodeMsg(long address, uint8_t type) {

	Message *msg = new Message(COMP_DISTRIBUTOR, type);

	switch(type) {

		case MSGTYPE_WAKEUP:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to node: %s", Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[COMP_NODE]->send(address, msg);

}

bool Distributor::send2CollectorMsg(long address, uint8_t type) {

	Message *msg = new Message(COMP_DISTRIBUTOR, type);

	switch(type) {

		case MSGTYPE_WAKEUP:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to collector: %s", Address::getString(address).c_str());
			break;

		case MSGTYPE_NODE: {
				NodeItem *node = nodeManager->getIdle(address);

				if (node != nullptr) {

					LOG_U(UI_UPDATE_DIST_NODE_LIST, node->address, PREBUSY);
					LOG_U(UI_UPDATE_DIST_LOG,
							"\"NODE\" msg sent to collector: %s with available node: %s",
						  Address::getString(address).c_str(),
						  Address::getString(node->address).c_str());

                    msg->setVariant(0, node->address);
                    msg->setVariant(1, node->id);

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, node->address);

				} else {
					collectorWaitingList.push_back(address);

					LOG_U(UI_UPDATE_DIST_LOG,
							"\"NODE\" msg sent to collector: %s with no available node",
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

	return connectors[COMP_COLLECTOR]->send(address, msg);

}

bool Distributor::sendWakeupMessage(Connector *connector) {

    if (Util::isMulticast()) {

        Message *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);
        connector->send(msg);

        LOG_U(UI_UPDATE_LOG,
              "\"WAKEUP\" message sent as multicast");

    } else {

        std::vector<long> list = connector->getAddressList();

        for (int i = 0; i < list.size(); i++) {

            Message *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

            connector->send(list[i], msg);

            LOG_U(UI_UPDATE_LOG,
                  "\"WAKEUP\" message sent to : %s", Address::getString(list[i]).c_str());

        }

        LOG_U(UI_UPDATE_DIST_LOG,
              "\"WAKEUP\" messages sent to network");
    }

    return true;
}

bool Distributor::sendWakeupMessagesAll() {

	sendWakeupMessage(connectors[COMP_NODE]);
	if (connectors[COMP_COLLECTOR] != connectors[COMP_NODE]) {
		sendWakeupMessage(connectors[COMP_COLLECTOR]);
	}
	return true;
}

bool Distributor::reset() {

	nodeManager->clear();
	collectorWaitingList.clear();
	collStartTime.reset();
	return true;

}

bool Distributor::onTimeOut(Component *component, NodeItem *node) {

	Message *msg = new Message(COMP_NODE, MSGTYPE_TIMEOUT);
	msg->setVariant(0, node->lastServedCollector);
    component->connectors[COMP_NODE]->put(node->address, msg);

	return true;
}


bool Distributor::onWakeup(Component *component) {

    return ((Distributor*)component)->sendWakeupMessage(component->connectors[COMP_NODE]);
}
