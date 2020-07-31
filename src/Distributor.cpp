//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"

Distributor *Distributor::instance = nullptr;

Distributor *Distributor::newInstance(const char* path, int interfaceOther, int interfaceNode) {

    if (!instance) {
        instance = new Distributor(path, interfaceOther, interfaceNode);
    }

    return instance;
}

Distributor::Distributor(const char *rootPath, int interfaceOther, int interfaceNode) :
        Component(rootPath) {

    host = new DistributorObject(getRootPath());

    addProcessHandler(COMP_COLLECTOR, MSGTYPE_ALIVE, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorIDMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_NODE, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorReadyMsg));

    addProcessHandler(COMP_NODE, MSGTYPE_ALIVE, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeIDMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_BUSY, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg));

    nodeManager = new NodeManager();

    collectorManager = new CollectorManager();

    collThread = std::thread(collProcessCB, this);

    pollThread = std::thread(pollProcessCB, this);

    initInterfaces(COMP_DISTRIBUTOR, interfaceOther, interfaceNode);
};

Distributor::~Distributor() {

    runPollThread = false;

    runCollThread = false;

    pollThread.join();

    collThread.join();

    delete nodeManager;

    delete collectorManager;

}

bool Distributor::clear() {

    nodeManager->clear();

    collectorManager->clear();

    return true;
}

void Distributor::pollProcessCB(Distributor *distributor) {

    distributor->pollProcess();
}

void Distributor::pollProcess() {

    int loop = 0;

    while(runPollThread) {

        std::this_thread::sleep_for(std::chrono::milliseconds (1000));

        if (!runPollThread) {
            break;
        }

        if (loop++ == 5) {

            loop = 0;
            sendWakeupMessagesAll(false);
        }
    }
}

void Distributor::collProcessCB(Distributor *distributor) {

    distributor->collProcess();
}

void Distributor::collProcess() {

    while(runCollThread) {

        std::this_thread::sleep_for(std::chrono::milliseconds (500));

        int nodeCount = nodeManager->getIdleCount();
        if (nodeCount == 0) {
            continue;
        }

        CollectorRequest *request = collectorManager->getRequest();
        if (request == nullptr) {
            continue;
        }

        std::vector<ComponentObject> nodes;
        long collID = request->collID;

        if (request->reqCount > nodeCount) {

            for (int i = 0; i < nodeCount; i++) {
                nodes.emplace_back(nodeManager->getIdle());
            }

            collectorManager->updateRequest(request->reqCount - nodeCount);

        } else {

            for (int i = 0; i < request->reqCount; i++) {
                nodes.emplace_back(nodeManager->getIdle());
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

bool Distributor::processCollectorAliveMsg(const ComponentObject& owner, Message *msg) {

    bool alreadyAdded = false;
    long collID = collectorManager->add(owner.getAddress(), alreadyAdded);
    if (alreadyAdded) {
        LOGS_I(getHost(), "Collector[%d] is already added", collID);
        notifyUI(NOTIFYSTATE_ONCE);
        return true;
    }

    LOGS_I(getHost(), "Collector at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(owner.getAddress()).c_str(), collID);

    return send2CollectorIDMsg(owner, collID);
}

bool Distributor::processCollectorIDMsg(const ComponentObject& owner, Message *msg) {

    collectorManager->setState(owner.getID(), COLLSTATE_IDLE);

    notifyUI(NOTIFYSTATE_ONCE);

    LOGS_I(getHost(), "Collector[%d] is confirmed", owner.getID());

    return true;
}

bool Distributor::processCollectorNodeMsg(const ComponentObject& owner, Message *msg) {

    collectorManager->addRequest(owner.getID(), (int)msg->getHeader().getVariant(0));

    collectorManager->setState(owner.getID(), COLLSTATE_BUSY);

    notifyUI(NOTIFYSTATE_ONCE);

    return true;
}

bool Distributor::processCollectorReadyMsg(const ComponentObject& owner, Message *msg) {

    collectorManager->setState(owner.getID(), COLLSTATE_IDLE);

    notifyUI(NOTIFYSTATE_ONCE);

    return true;
}

bool Distributor::processNodeAliveMsg(const ComponentObject& owner, Message *msg) {

    bool alreadyAdded = false;
    long nodeID = nodeManager->add(owner.getAddress(), alreadyAdded);
    if (alreadyAdded) {
        LOGS_I(getHost(), "Node[%d] is already added", nodeID);
        notifyUI(NOTIFYSTATE_ONCE);
        return true;
    }

    LOGS_I(getHost(), "Node at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(owner.getAddress()).c_str(), nodeID);

    return send2NodeIDMsg(owner, nodeID);
}

bool Distributor::processNodeIDMsg(const ComponentObject& owner, Message *msg) {

    nodeManager->setState(owner.getID(), NODESTATE_IDLE);

    notifyUI(NOTIFYSTATE_ONCE);

    LOGS_I(getHost(), "Node[%d] is confirmed", owner.getID());

    return true;
}

bool Distributor::processNodeBusyMsg(const ComponentObject& owner, Message *msg) {

    if (!nodeManager->isExist(owner.getID())) {
        LOGS_I(getHost(),
               "Could not found a node with id : %s",
               InterfaceTypes::getAddressString(owner.getID()).c_str());
        return false;
    }

    int collID = (int) msg->getHeader().getVariant(0);

    nodeManager->setState(owner.getID(), NODESTATE_BUSY);

    nodeManager->setAssigned(owner.getID(), collID, collectorManager->get(collID)->getAddress());

    notifyUI(NOTIFYSTATE_ONCE);

    LOGS_I(getHost(), "Node[%d] is Busy with Collector[%d]\'s process", owner.getID(), collID);


    return send2NodeProcessMsg(owner);
}

bool Distributor::processNodeReadyMsg(const ComponentObject& owner, Message *msg) {

    if (!nodeManager->isExist(owner.getID())) {
        LOGS_I(getHost(),
               "Could not found a node with id : %s",
               InterfaceTypes::getAddressString(owner.getID()).c_str());
        return false;
    }

    nodeManager->setState(owner.getID(), NODESTATE_IDLE);

    nodeManager->setAssigned(owner.getID(), 0, 0);

    notifyUI(NOTIFYSTATE_ONCE);

    LOGS_I(getHost(), "Node[%d] is IDLE", owner.getID());

    return true;
}

bool Distributor::send2CollectorWakeupMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_COLLECTOR, MSGTYPE_WAKEUP);

    return send(target, msg);
}

bool Distributor::send2CollectorIDMsg(const ComponentObject& target, long id) {

    auto *msg = new Message(getHost(), COMP_COLLECTOR, MSGTYPE_ID);

    msg->getHeader().setVariant(0, id);

    return send(target, msg);
}

bool Distributor::send2CollectorNodeMsg(const ComponentObject& target, std::vector<ComponentObject>& nodes) {

    auto *msg = new Message(getHost(), COMP_COLLECTOR, MSGTYPE_NODE);

    msg->getData().setStreamFlag(STREAM_COMPONENT);
    msg->getData().setComponentList(nodes);

    return send(target, msg);
}

bool Distributor::send2NodeWakeupMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_NODE, MSGTYPE_WAKEUP);

    return send(target, msg);
}

bool Distributor::send2NodeIDMsg(const ComponentObject& target, long id) {

    auto *msg = new Message(getHost(), COMP_NODE, MSGTYPE_ID);

    msg->getHeader().setVariant(0, id);

    return send(target, msg);
}

bool Distributor::send2NodeProcessMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_NODE, MSGTYPE_PROCESS);

    return send(target, msg);
}

bool Distributor::sendWakeupMessage(COMPONENT targetType) {

    auto target = ComponentObject(targetType);

    if (isSupportMulticast(targetType)) {

        auto *msg = new Message(getHost(), targetType, MSGTYPE_WAKEUP);

        //TODO think multiple interfaces again
        target.setAddress(getInterfaceMulticastAddress(targetType));
        send(target, msg);

    } else {

        std::vector<long> list = getDevice(targetType)->getAddressList();

        for (auto &address : list) {

            auto *msg = new Message(getHost(), targetType, MSGTYPE_WAKEUP);

            target.setAddress(address);

            send(target, msg);
        }
    }

    return true;
}

bool Distributor::sendWakeupMessagesAll(bool clear) {

    if (clear) {
        this->clear();
    }

    sendWakeupMessage(COMP_NODE);

    if (getDevice(COMP_NODE) != getDevice(COMP_COLLECTOR)) {
        sendWakeupMessage(COMP_COLLECTOR);
    }

    return true;
}
