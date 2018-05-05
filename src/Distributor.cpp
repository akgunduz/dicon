//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DeviceList.h"

Distributor *Distributor::instance = NULL;

Distributor *Distributor::newInstance(const char* path) {

    if (instance) {
        return instance;
    }

    instance = new Distributor(path);
    return instance;
}

Distributor::Distributor(const char *rootPath) :
        Component(COMP_DISTRIBUTOR, rootPath) {

    processMsg[COMP_COLLECTOR][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg);
    processMsg[COMP_NODE][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg);
    processMsg[COMP_NODE][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg);
    processMsg[COMP_NODE][MSGTYPE_BUSY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg);
    processMsg[COMP_NODE][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeIDMsg);

    nodeManager = new NodeManager();

    collectorManager = new CollectorManager();
};

Distributor::~Distributor() {

    delete nodeManager;

    delete collectorManager;
}

bool Distributor::processCollectorAliveMsg(ComponentObject owner, long address, Message *msg) {

    if (!collectorManager->add(address)) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(address), 0});

    LOGS_I(getHost(), "Collector at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(address).c_str(), collectorManager->getID(address));

    return send2CollectorIDMsg(owner, address, collectorManager->getID(address));
}

bool Distributor::processCollectorNodeMsg(ComponentObject owner, long address, Message *msg) {

    long nodeAddress = nodeManager->getIdle();

    if (nodeAddress > 0) {

        LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(nodeAddress), PREBUSY});

        LOGS_I(getHost(), "Node[%d] is assigned to Collector[%d]", nodeManager->getID(nodeAddress), collectorManager->getID(address));

        return send2CollectorNodeMsg(owner, address, msg->getData()->getJobDir(), nodeAddress, nodeManager->getID(nodeAddress));

    } else {

        collectorManager->addWaiting(address, msg->getData()->getJobDir());

        LOGS_I(getHost(), "No node can be assigned to Collector[%d], moving it to Wait List with size %d",
               collectorManager->getID(address), collectorManager->getWaitingCount());
    }

    return false;
}

bool Distributor::processNodeAliveMsg(ComponentObject owner, long address, Message *msg) {

    if (!nodeManager->add(address)) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(address), IDLE});

    LOGS_I(getHost(), "Node at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(address).c_str(), nodeManager->getID(address));

    return send2NodeIDMsg(owner, address, nodeManager->getID(address));
}

bool Distributor::processNodeReadyMsg(ComponentObject owner, long address, Message *msg) {

    if (!nodeManager->setState(address, IDLE)) {
        LOGS_I(getHost(),
               "Could not found a node with address : %s",
               InterfaceTypes::getAddressString(address).c_str());
        return false;
    }

    long collAddress = msg->getHeader()->getVariant(0);

    LOGS_I(getHost(), "Node[%d] is done with Collector[%d]\'s process, setting to IDLE",
           nodeManager->getID(address), collectorManager->getID(collAddress));

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(collAddress), 0});
    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(address), IDLE});

    return processWaitingList(owner, msg);
}

bool Distributor::processNodeIDMsg(ComponentObject owner, long address, Message *msg) {

    return processWaitingList(owner, msg);
}

bool Distributor::processNodeBusyMsg(ComponentObject owner, long address, Message *msg) {

    nodeManager->setState(address, BUSY);

    long collAddress = msg->getHeader()->getVariant(0);

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(collAddress), nodeManager->getID(address)});
    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(address), BUSY});

    LOGS_I(getHost(), "Node[%d] is Busy with Collector[%d]\'s job", nodeManager->getID(address), collectorManager->getID(collAddress));

    return true;
}


bool Distributor::processWaitingList(ComponentObject owner, Message *msg) {

    TypeWaitingCollector collector = collectorManager->getWaiting();

    if (collector.first == 0) {
        return false;
    }

    long nodeAddress = nodeManager->getIdle();

    if (nodeAddress > 0) {

        LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(nodeAddress), PREBUSY});

        LOGS_I(getHost(), "Node[%d] is assigned to Collector[%d] from Wait List with size : %d",
               nodeManager->getID(nodeAddress), collectorManager->getID(collector.first), collectorManager->getWaitingCount());

        return send2CollectorNodeMsg(*collectorManager->get(collector.first),
                                     collector.first, msg->getData()->getJobDir(), nodeAddress, nodeManager->getID(nodeAddress));

    } else {

        collectorManager->addWaiting(collector.first, msg->getData()->getJobDir());

        LOGS_I(getHost(), "No node can be assigned to Collector[%d], moving it to Wait List with size %d",
               collectorManager->getID(collector.first), collectorManager->getWaitingCount());
    }


    return false;
}


bool Distributor::send2CollectorWakeupMsg(ComponentObject target, long address) {

    auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

    return send(target, address, msg);
}

bool Distributor::send2CollectorIDMsg(ComponentObject target, long address, int id) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    msg->getHeader()->setVariant(0, id);

    return send(target, address, msg);
}

bool Distributor::send2CollectorNodeMsg(ComponentObject target, long address, const char *jobDir, long nodeAddress, int nodeID) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getHeader()->setVariant(0, nodeID);
    msg->getHeader()->setVariant(1, nodeAddress);
    msg->getData()->setJobDir(jobDir);

    return send(target, address, msg);
}

bool Distributor::send2NodeWakeupMsg(ComponentObject target, long address) {

    auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

    return send(target, address, msg);
}

bool Distributor::send2NodeIDMsg(ComponentObject target, long address, int id) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    msg->getHeader()->setVariant(0, id);

    return send(target, address, msg);
}

bool Distributor::sendWakeupMessage(ComponentObject component) {

    if (isSupportMulticast(component)) {

        auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);
        send(component, msg);

    } else {

        std::vector<long> list = getAddressList(component);

        for (int i = 0; i < list.size(); i++) {

            auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

            send(component, list[i], msg);
        }
    }

    return true;
}

bool Distributor::sendWakeupMessagesAll(bool clear) {

    if (clear) {
        this->clear();
    }

    sendWakeupMessage(ComponentObject(COMP_NODE, getRootPath()));

    if (DeviceList::getInstance()->isActiveDifferent()) {
        sendWakeupMessage(ComponentObject(COMP_COLLECTOR, getRootPath()));
    }

    return true;
}

bool Distributor::clear() {

    nodeManager->clear();

    collectorManager->clear();

    return true;

}
