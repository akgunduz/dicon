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

bool Distributor::processCollectorAliveMsg(ComponentObject owner, Message *msg) {

    if (!collectorManager->add(owner.getAddress())) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(owner.getAddress()), 0});

    LOGS_I(getHost(), "Collector at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(owner.getAddress()).c_str(), collectorManager->getID(owner.getAddress()));

    return send2CollectorIDMsg(owner, collectorManager->getID(owner.getAddress()));
}

bool Distributor::processCollectorNodeMsg(ComponentObject owner, Message *msg) {

    return processWaitingList(owner.getAddress(), msg->getHeader()->getVariant(0), msg->getData()->getJobDir());
}

bool Distributor::processNodeAliveMsg(ComponentObject owner, Message *msg) {

    if (!nodeManager->add(owner.getAddress())) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(owner.getAddress()), IDLE});

    LOGS_I(getHost(), "Node at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(owner.getAddress()).c_str(), nodeManager->getID(owner.getAddress()));

    return send2NodeIDMsg(owner, nodeManager->getID(owner.getAddress()));
}

bool Distributor::processNodeReadyMsg(ComponentObject owner, Message *msg) {

    if (!nodeManager->setState(owner.getAddress(), IDLE)) {
        LOGS_I(getHost(),
               "Could not found a node with address : %s",
               InterfaceTypes::getAddressString(owner.getAddress()).c_str());
        return false;
    }

    long collAddress = msg->getHeader()->getVariant(0);
    long collUnservedCount = msg->getHeader()->getVariant(1);

    LOGS_I(getHost(), "Node[%d] is done with Collector[%d]\'s process, setting to IDLE",
           nodeManager->getID(owner.getAddress()), collectorManager->getID(collAddress));

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(collAddress), 0});
    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(owner.getAddress()), IDLE});

    return processWaitingList(collAddress, collUnservedCount, msg->getData()->getJobDir());
}

bool Distributor::processNodeIDMsg(ComponentObject owner, Message *msg) {

    //return processWaitingList(0, 0, msg->getData()->getJobDir());
    return true;
}

bool Distributor::processNodeBusyMsg(ComponentObject owner, Message *msg) {

    nodeManager->setState(owner.getAddress(), BUSY);

    long collAddress = msg->getHeader()->getVariant(0);

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(collAddress), nodeManager->getID(owner.getAddress())});
    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(owner.getAddress()), BUSY});

    LOGS_I(getHost(), "Node[%d] is Busy with Collector[%d]\'s job", nodeManager->getID(owner.getAddress()), collectorManager->getID(collAddress));

    return send2NodeProcessMsg(owner, msg->getData()->getJobDir(),
                                   msg->getData()->getExecutorID(),
                                   msg->getData()->getExecutor(),
                                   msg->getData()->getFileList(),
                                   collectorManager->getID(collAddress),
                                   collAddress);
}


bool Distributor::processWaitingList(long collAddress, long collUnservedCount, const char* jobDir) {

    while(collUnservedCount--) {

        collectorManager->addWaiting(collAddress, jobDir);
    }

    LOGS_I(getHost(), "Waiting list size : %d", collectorManager->getWaitingCount());

    if (collectorManager->getWaitingCount() > 0) {

        long nodeAddress = nodeManager->getIdle();

        if (nodeAddress > 0) {

            TypeWaitingCollector collector = collectorManager->getWaiting();

            //LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {collectorManager->getID(collector.first), nodeManager->getID(nodeAddress)});
            LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeManager->getID(nodeAddress), PREBUSY});

            LOGS_I(getHost(), "Node[%d] is assigned to Collector[%d] from Wait List with size : %d",
                   nodeManager->getID(nodeAddress), collectorManager->getID(collector.first), collectorManager->getWaitingCount());

            return send2CollectorNodeMsg(*collectorManager->get(collector.first),
                                         jobDir, nodeAddress, nodeManager->getID(nodeAddress));

        }
    }

    return true;
}

bool Distributor::send2CollectorWakeupMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

    return send(target, msg);
}

bool Distributor::send2CollectorIDMsg(ComponentObject target, int id) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    msg->getHeader()->setVariant(0, id);

    return send(target, msg);
}

bool Distributor::send2CollectorNodeMsg(ComponentObject target, const char *jobDir, long nodeAddress, int nodeID) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getHeader()->setVariant(0, nodeID);
    msg->getHeader()->setVariant(1, nodeAddress);
    msg->getData()->setJobDir(jobDir);

    return send(target, msg);
}

bool Distributor::send2NodeWakeupMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

    return send(target, msg);
}

bool Distributor::send2NodeIDMsg(ComponentObject target, int id) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    msg->getHeader()->setVariant(0, id);

    return send(target, msg);
}

bool Distributor::send2NodeProcessMsg(ComponentObject target,
                                   const char* jobDir, long executionID, const char *executor,
                                   TypeFileInfoList *fileList, int collID, long collAddress) {

    auto *msg = new Message(getHost(), MSGTYPE_PROCESS);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executionID, executor);
    msg->getData()->addFileList(fileList);

    msg->getHeader()->setVariant(0, collID);
    msg->getHeader()->setVariant(1, collAddress);

    return send(target, msg);
}

bool Distributor::sendWakeupMessage(ComponentObject component) {

    if (isSupportMulticast(component)) {

        auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

        component.setAddress(getInterfaceMulticastAddress(component));
        send(component, msg);

    } else {

        std::vector<long> list = getAddressList(component);

        for (int i = 0; i < list.size(); i++) {

            auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

            component.setAddress(list[i]);
            send(component, msg);
        }
    }

    return true;
}

bool Distributor::sendWakeupMessagesAll(bool clear) {

    if (clear) {
        this->clear();
    }

    sendWakeupMessage(COMP_NODE);

    if (DeviceList::getInstance()->isActiveDifferent()) {
        sendWakeupMessage(COMP_COLLECTOR);
    }

    return true;
}

bool Distributor::clear() {

    nodeManager->clear();

    collectorManager->clear();

    return true;

}
