//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DeviceList.h"

Distributor::Distributor(const char *rootPath, double backupRate) :
            Component(Unit(COMP_DISTRIBUTOR), rootPath){

	nodeManager = new NodeManager(this, onTimeOut, onWakeup, backupRate);

    LOG_U(UI_UPDATE_DIST_ADDRESS, getInterfaceAddress(COMP_COLLECTOR), getInterfaceAddress(COMP_NODE));
};

Distributor::~Distributor() {

	delete nodeManager;
}

bool Distributor::processDistributorMsg(long address, Message *msg) {
	return false;
}

bool Distributor::processCollectorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getHeader()->getType()) {

		case MSGTYPE_ALIVE:
			LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
			break;

		case MSGTYPE_NODE:
			status = send2CollectorMsg(address, MSGTYPE_NODE);
			break;

		default:
			break;
	}

	delete msg;
	return status;

}

bool Distributor::processNodeMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getHeader()->getType()) {

		case MSGTYPE_READY:

			nodeManager->setIdle(NodeInfo(address, msg->getHeader()->getOwner().getArch()));

			if (collectorWaitingList.size() > 0) {

				long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
                      "Processing a collector from the waiting list: %s",
					  InterfaceTypes::getAddressString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_NODE);

			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

			break;

		case MSGTYPE_ALIVE:

			if (!nodeManager->validate(NodeInfo(address, msg->getHeader()->getOwner().getArch()))
					&& collectorWaitingList.size() > 0) {

                long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
						"Processing a collector from the waiting list: %s",
					  InterfaceTypes::getAddressString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_NODE);
			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

			break;

		case MSGTYPE_BUSY:

			nodeManager->setBusy(address);
			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, BUSY);


			status = true;
			break;

		case MSGTYPE_TIMEOUT:

			nodeManager->remove(address);
			LOG_U(UI_UPDATE_DIST_NODE_LIST, address, REMOVE);

			status = send2CollectorMsg(msg->getHeader()->getVariant(0), MSGTYPE_NODE);

			break;

		default :
			break;
	}

	delete msg;
	return status;

}

bool Distributor::send2NodeMsg(long address, MSG_TYPE type) {

	Message *msg = new Message(COMP_DISTRIBUTOR, type);

	switch(type) {

		case MSGTYPE_WAKEUP:
			break;

		default:
			delete msg;
			return false;

	}

	return send(COMP_NODE, address, msg);

}

bool Distributor::send2CollectorMsg(long address, MSG_TYPE type) {

	Message *msg = new Message(COMP_DISTRIBUTOR, type);

	switch(type) {

		case MSGTYPE_WAKEUP:
			break;

		case MSGTYPE_NODE: {

				NodeObject *node = nodeManager->getIdle();

				if (node != nullptr) {

					LOG_U(UI_UPDATE_DIST_NODE_LIST, node->getNode()->getAddress(), PREBUSY);
					LOG_U(UI_UPDATE_DIST_LOG, "Available node: %s",
                          InterfaceTypes::getAddressString(node->getNode()->getAddress()).c_str());

                    msg->getHeader()->setVariant(0, node->getNode()->getAddress());
                    msg->getHeader()->setVariant(1, node->getNode()->getArch());

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, node->getNode()->getAddress());

				} else {

					collectorWaitingList.push_back(address);

					LOG_U(UI_UPDATE_DIST_LOG, "No available node");

                    msg->getHeader()->setVariant(0, 0);

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
				}

			}
			break;

		default:
			delete msg;
			return false;
	}

	return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::sendWakeupMessage(COMPONENT component) {

    if (isSupportMulticast(component)) {

        Message *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);
        send(component, msg);

    } else {

        std::vector<long> list = getAddressList(component);

        for (int i = 0; i < list.size(); i++) {

            Message *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

            send(component, list[i], msg);
        }
    }

    return true;
}

bool Distributor::sendWakeupMessagesAll() {

	sendWakeupMessage(COMP_NODE);

    if (DeviceList::getInstance()->isActiveDifferent()) {
        sendWakeupMessage(COMP_COLLECTOR);
    }

	return true;
}

bool Distributor::reset() {

	nodeManager->clear();
	collectorWaitingList.clear();
	return true;

}

bool Distributor::onTimeOut(Component *component, NodeObject *node) {

	Message *msg = new Message(COMP_NODE, MSGTYPE_TIMEOUT);
    component->put(COMP_NODE, node->getNode()->getAddress(), msg);

	return true;
}


bool Distributor::onWakeup(Component *component) {

    return ((Distributor*)component)->sendWakeupMessage(COMP_NODE);
}
