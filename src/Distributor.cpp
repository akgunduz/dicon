//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DeviceList.h"

Distributor* Distributor::instance = NULL;

Distributor *Distributor::newInstance() {

    if (instance) {
        return instance;
    }

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(NULL, 0), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    instance = new Distributor(path);
    return instance;
}

Distributor::Distributor(const char *rootPath) :
            Component(COMP_DISTRIBUTOR, rootPath){

	processMsg[COMP_COLLECTOR][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg);
	processMsg[COMP_COLLECTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg);
	processMsg[COMP_NODE][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg);
	processMsg[COMP_NODE][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg);
	processMsg[COMP_NODE][MSGTYPE_BUSY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg);
	//processMsg[COMP_NODE][MSGTYPE_TIMEOUT] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeTimeoutMsg);

	nodeManager = new NodeManager();

	collectorManager = new CollectorManager();

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

    long nodeAddress = nodeManager->getIdle();

    if (nodeAddress > 0) {

        LOG_U(UI_UPDATE_DIST_NODE_LIST, nodeAddress, PREBUSY);
        LOG_U(UI_UPDATE_DIST_LOG, "Available node: %s",
              InterfaceTypes::getAddressString(nodeAddress).c_str());

        LOG_U(UI_UPDATE_DIST_COLL_LIST, address, nodeAddress);

        return send2CollectorNodeMsg(address, msg->getData()->getJobDir(), nodeAddress);

    } else {

        collectorManager->add(address, msg->getData()->getJobDir());

        LOG_U(UI_UPDATE_DIST_LOG, "No available node");
        LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
    }

    return false;
}

bool Distributor::processNodeReadyMsg(long address, Message *msg) {

	if (!nodeManager->setState(address, IDLE)) {
        LOG_U(UI_UPDATE_DIST_LOG,
              "Could not found a node with address : %s",
              InterfaceTypes::getAddressString(address).c_str());

	}

	//TODO     urgent!!!!!!!!

    bool status = false;

    TypeWaitingCollector collector = collectorManager->get();

	if (collector.first > 0) {

		LOG_U(UI_UPDATE_DIST_LOG,
			  "Processing a collector from the waiting list: %s",
			  InterfaceTypes::getAddressString(collector.first).c_str());

		status = send2CollectorNodeMsg(collector.first, collector.second, address);

	} else {

		status = true;
	}

	LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

	return status;
}

bool Distributor::processNodeAliveMsg(long address, Message *msg) {

    if (!nodeManager->add(address)) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_LOG, "Node at address : %s added to the list",
          InterfaceTypes::getAddressString(address).c_str());

    bool status = false;

    TypeWaitingCollector collector = collectorManager->get();

    if (collector.first) {

        LOG_U(UI_UPDATE_DIST_LOG,
              "Processing a collector from the waiting list: %s",
              InterfaceTypes::getAddressString(collector.first).c_str());

        status = send2CollectorNodeMsg(collector.first, collector.second, address);

    } else {

        status = true;
    }

    LOG_U(UI_UPDATE_DIST_NODE_LIST, address, IDLE);

    return status;

}

bool Distributor::processNodeBusyMsg(long address, Message *msg) {

	nodeManager->setState(address, BUSY);

	LOG_U(UI_UPDATE_DIST_NODE_LIST, address, BUSY);

	return true;
}

bool Distributor::send2NodeWakeupMsg(long address) {

	auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

	return send(COMP_NODE, address, msg);
}

bool Distributor::send2CollectorWakeupMsg(long address) {

    auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP);

    return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::send2CollectorNodeMsg(long address, const char* jobDir, long nodeAddress) {

	auto *msg = new Message(COMP_DISTRIBUTOR, MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getHeader()->setVariant(0, nodeAddress);
    msg->getData()->setJobDir(jobDir);

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

	collectorManager->clear();

	return true;

}
