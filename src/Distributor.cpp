//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"

Distributor *Distributor::instance = NULL;

Distributor *Distributor::newInstance(const char* path) {

    if (instance) {
        return instance;
    }

    instance = new Distributor(path);
    return instance;
}

Distributor::Distributor(const char *rootPath) :
        Component(rootPath) {

    host = new DistributorObject(getRootPath());

    processMsg[COMP_COLLECTOR][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorIDMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg);

    processMsg[COMP_NODE][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg);
    processMsg[COMP_NODE][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeIDMsg);
    processMsg[COMP_NODE][MSGTYPE_BUSY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg);
    processMsg[COMP_NODE][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg);


    nodeManager = new NodeManager();

    collectorManager = new CollectorManager();

    collThread = std::thread(collProcessCB, this);

    initInterfaces(COMP_DISTRIBUTOR);
};

Distributor::~Distributor() {

    runCollThread = false;

    collThread.join();

    delete nodeManager;

    delete collectorManager;

}

bool Distributor::clear() {

    nodeManager->clear();

    collectorManager->clear();

    return true;
}

void Distributor::collProcessCB(Distributor *distributor) {

    distributor->collProcess();
}

void Distributor::collProcess() {

    while(runCollThread) {

        usleep(500000);

        int nodeCount = nodeManager->getIdleCount();
        if (nodeCount == 0) {
            continue;
        }

        CollectorRequest *request = collectorManager->getRequest();
        if (request == nullptr) {
            continue;
        }

        std::vector<ComponentObject> nodes;
        int collID = request->collID;

        if (request->reqCount > nodeCount) {

            for (int i = 0; i < nodeCount; i++) {
                nodes.emplace_back((ComponentObject)nodeManager->getIdle());
            }

            collectorManager->updateRequest(request->reqCount - nodeCount);

        } else {

            for (int i = 0; i < request->reqCount; i++) {
                nodes.emplace_back((ComponentObject)nodeManager->getIdle());
            }

            collectorManager->removeRequest();
        }

        send2CollectorNodeMsg(*collectorManager->get(collID), nodes);
    }
}

CollectorManager *Distributor::getCollectors() const {
    return collectorManager;
}

NodeManager *Distributor::getNodes() const {
    return nodeManager;
}

bool Distributor::processCollectorAliveMsg(ComponentObject owner, Message *msg) {

    int collID = collectorManager->add(owner.getAddress());
    if (!collID) {
        return false;
    }

    LOGS_I(getHost(), "Collector at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(owner.getAddress()).c_str(), collID);

    return send2CollectorIDMsg(owner, collID);
}

bool Distributor::processCollectorIDMsg(ComponentObject owner, Message *msg) {

    collectorManager->setState(owner.getID(), COLLSTATE_IDLE);

    LOG_U(UI_UPDATE_DIST_COLL_LISTITEM, std::vector<long> {owner.getID(), 0});

    LOGS_I(getHost(), "Collector[%d] is confirmed", owner.getID());

    return true;
}

bool Distributor::processCollectorNodeMsg(ComponentObject owner, Message *msg) {

    collectorManager->addRequest(owner.getID(), msg->getHeader()->getVariant(0));

    return true;
}

bool Distributor::processNodeAliveMsg(ComponentObject owner, Message *msg) {

    int nodeID = nodeManager->add(owner.getAddress());
    if (!nodeID) {
        return false;
    }

    LOGS_I(getHost(), "Node at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(owner.getAddress()).c_str(), nodeID);

    return send2NodeIDMsg(owner, nodeID);
}

bool Distributor::processNodeIDMsg(ComponentObject owner, Message *msg) {

    nodeManager->setState(owner.getID(), NODESTATE_IDLE);

    LOG_U(UI_UPDATE_DIST_NODE_LISTITEM, std::vector<long> {owner.getID(), nodeManager->getState(owner.getID())});

    LOGS_I(getHost(), "Node[%d] is confirmed", owner.getID());

    return true;
}

bool Distributor::processNodeReadyMsg(ComponentObject owner, Message *msg) {

    if (!nodeManager->isExist(owner.getID())) {
        LOGS_I(getHost(),
               "Could not found a node with id : %s",
               InterfaceTypes::getAddressString(owner.getID()).c_str());
        return false;
    }

    nodeManager->setState(owner.getID(), NODESTATE_IDLE);

    long collAddress = msg->getHeader()->getVariant(0);

    //TODO
//    long collUnservedCount = msg->getHeader()->getVariant(1);

//    LOGS_I(getHost(), "Node[%d] is Done with Collector[%d]\'s process",
//           nodeManager->getID(owner.getAddress()), collectorManager->getID(collAddress));
//
//    collectorManager->detachNode(collAddress);
//
//    LOG_U(UI_UPDATE_DIST_COLL_LISTITEM, std::vector<long> {collectorManager->getID(collAddress), 0});
//    LOG_U(UI_UPDATE_DIST_NODE_LISTITEM, std::vector<long> {nodeManager->getID(owner.getAddress()), NODESTATE_IDLE});
//
//    return processWaitingList(collAddress, collUnservedCount, msg->getData()->getJobDir());
}

bool Distributor::processNodeBusyMsg(ComponentObject owner, Message *msg) {

    if (!nodeManager->isExist(owner.getID())) {
        LOGS_I(getHost(),
               "Could not found a node with id : %s",
               InterfaceTypes::getAddressString(owner.getID()).c_str());
        return false;
    }

    nodeManager->setState(owner.getID(), NODESTATE_BUSY);

    long collAddress = msg->getHeader()->getVariant(0);

//    collectorManager->attachNode(collAddress, *nodeManager->get(owner.getAddress()));
//
//    LOG_U(UI_UPDATE_DIST_COLL_LISTITEM, std::vector<long> {collectorManager->getID(collAddress), nodeManager->getID(owner.getAddress())});
//    LOG_U(UI_UPDATE_DIST_NODE_LISTITEM, std::vector<long> {nodeManager->getID(owner.getAddress()), NODESTATE_BUSY});
//
//    LOGS_I(getHost(), "Node[%d] is Busy with Collector[%d]\'s process", nodeManager->getID(owner.getAddress()), collectorManager->getID(collAddress));
//
//    return send2NodeProcessMsg(owner, msg->getData()->getJobDir(),
//                                   msg->getData()->getExecutorID(),
//                                   msg->getData()->getExecutor(),
//                                   msg->getData()->getFileList(),
//                                   collectorManager->getID(collAddress),
//                                   collAddress);
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

bool Distributor::send2CollectorNodeMsg(ComponentObject target, std::vector<ComponentObject>& nodes) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_COMPONENT);
    msg->getData()->setComponentList(nodes);

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

//    msg->getData()->setStreamFlag(STREAM_INFO);
//    msg->getData()->setJobDir(jobDir);
//    msg->getData()->setExecutor(executionID, executor);
//    msg->getData()->addFileList(fileList);

//    msg->getHeader()->setVariant(0, collID);
//    msg->getHeader()->setVariant(1, collAddress);

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
