//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DeviceList.h"

Distributor::Distributor(const char *rootPath, double backupRate) :
            Component(COMP_DISTRIBUTOR, rootPath){

	processMsg[COMP_COLLECTOR][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg);
	processMsg[COMP_COLLECTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg);
	processMsg[COMP_NODE][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg);
	processMsg[COMP_NODE][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg);
	processMsg[COMP_NODE][MSGTYPE_BUSY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg);
	processMsg[COMP_NODE][MSGTYPE_TIMEOUT] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeTimeoutMsg);

	nodeManager = new NodeManager(this, onTimeOut, onWakeup, backupRate);

    LOG_U(UI_UPDATE_DIST_ADDRESS, getInterfaceAddress(COMP_COLLECTOR), getInterfaceAddress(COMP_NODE));
};

Distributor::~Distributor() {

	delete nodeManager;
}

bool Distributor::processCollectorAliveMsg(long address, Message *msg) {

    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
    return true;
}

bool Distributor::processCollectorNodeMsg(long address, Message *msg) {

	return send2CollectorNodeMsg(address);
}

bool Distributor::processNodeReadyMsg(long address, Message *msg) {

	bool status = false;

	nodeManager->setIdle(address);

	if (collectorWaitingList.size() > 0) {

		long collectorAddress = collectorWaitingList.front();
		collectorWaitingList.pop_front();

		LOG_U(UI_UPDATE_DIST_LOG,
			  "Processing a collector from the waiting list: %s",
			  InterfaceTypes::getAddressString(collectorAddress).c_str());

		status = send2CollectorNodeMsg(collectorAddress);

	} else {

		status = true;
	}

	LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

	return status;
}

bool Distributor::processNodeAliveMsg(long address, Message *msg) {

	bool status = false;

	if (!nodeManager->validate(address)
		&& collectorWaitingList.size() > 0) {

		long collectorAddress = collectorWaitingList.front();
		collectorWaitingList.pop_front();

		LOG_U(UI_UPDATE_DIST_LOG,
			  "Processing a collector from the waiting list: %s",
			  InterfaceTypes::getAddressString(collectorAddress).c_str());

		status = send2CollectorNodeMsg(collectorAddress);
	} else {

		status = true;
	}

	LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

	return status;
}

bool Distributor::processNodeBusyMsg(long address, Message *msg) {

	nodeManager->setBusy(address);
	LOG_U(UI_UPDATE_DIST_NODE_LIST, address, BUSY);

	return true;
}

bool Distributor::processNodeTimeoutMsg(long address, Message *msg) {

	nodeManager->remove(address);
	LOG_U(UI_UPDATE_DIST_NODE_LIST, address, REMOVE);

	return send2CollectorNodeMsg(msg->getHeader()->getVariant(0));
}

bool Distributor::send2NodeWakeupMsg(long address) {

	auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

	return send(COMP_NODE, address, msg);
}

bool Distributor::send2CollectorWakeupMsg(long address) {

    auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

    return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::send2CollectorNodeMsg(long address) {

	auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_NODE);

    NodeObject *node = nodeManager->getIdle();

    if (node != nullptr) {

        LOG_U(UI_UPDATE_DIST_NODE_LIST, node->getAddress(), PREBUSY);
        LOG_U(UI_UPDATE_DIST_LOG, "Available node: %s",
              InterfaceTypes::getAddressString(node->getAddress()).c_str());

        msg->getHeader()->setVariant(0, node->getAddress());

        LOG_U(UI_UPDATE_DIST_COLL_LIST, address, node->getAddress());

    } else {

        collectorWaitingList.push_back(address);

        LOG_U(UI_UPDATE_DIST_LOG, "No available node");

        msg->getHeader()->setVariant(0, 0);

        LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
    }

	return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::sendWakeupMessage(COMPONENT component) {

    if (isSupportMulticast(component)) {

        auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);
        send(component, msg);

    } else {

        std::vector<long> list = getAddressList(component);

        for (int i = 0; i < list.size(); i++) {

            auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

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

    auto *msg = new Message(COMP_NODE, MSGTYPE_TIMEOUT);

    component->put(COMP_NODE, node->getAddress(), msg);

	return true;
}


bool Distributor::onWakeup(Component *component) {

    return ((Distributor*)component)->sendWakeupMessage(COMP_NODE);
}
