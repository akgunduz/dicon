//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DistributorHost.h"
#include "UtilNet.h"
#include "NodeManager.h"
#include "CollectorManager.h"
#include "Log.h"

Distributor::Distributor(int _commInterfaceOther, int _commInterfaceNode, bool _autoWake) :
        Component(std::make_shared<DistributorHost>()), autoWake(_autoWake) {

    addProcessHandler(COMP_COLLECTOR, MSGTYPE_ALIVE, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorIDMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_NODE, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorReadyMsg));

    addProcessHandler(COMP_NODE, MSGTYPE_ALIVE, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeIDMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_BUSY, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg));

    if (!initInterfaces(COMP_DISTRIBUTOR, _commInterfaceOther, _commInterfaceNode)) {
        LOGS_E(getHost(), "Distributor could not initialized");
        return;
    }

    nodeManager = new NodeManager(getHost(), !autoWake);

    collectorManager = new CollectorManager(getHost(), true);

    collThread = std::thread([](Distributor *distributor) {

        distributor->collProcess();

    }, this);

    pollThread = std::thread([](Distributor *distributor){

            distributor->pollProcess();

    }, this);

    initialized = true;

    LOGS_T(getHost(), "Distributor is initialized");
}

volatile bool Distributor::getAutoWake() const {

    return autoWake;
}

Distributor::~Distributor() {

    LOGP_T("Deallocating Distributor");

    if (!initialized) {
        return;
    }

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

void Distributor::pollProcess() {

    int loop = 0;

    while(runPollThread) {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (loop++ < WAKEUP_INTERVAL) {
            continue;
        }

        loop = 0;

        if (autoWake) {
            sendWakeupMessagesAll(false);
        }
    }
}

void Distributor::collProcess() {

    while(runCollThread) {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        nodeManager->updateDead();

        auto nodeCount = nodeManager->getIdleCount();
        if (nodeCount == 0) {
            continue;
        }

        collectorManager->updateDead();

        auto busyDeadCount = nodeManager->getBusyDeadCount();

        if (busyDeadCount) {

            processBusyDead(busyDeadCount, nodeCount);
            continue;
        }

        auto *request = collectorManager->getRequest();
        if (request == nullptr) {
          //  LOGS_T(getHost(), "Request Queue is empty");
            continue;
        }

        TypeComponentUnitList nodes;

        auto coll = std::static_pointer_cast<CollectorUnit>(collectorManager->get(request->collID));

        LOGS_D(getHost(), "Request is in progress for Collector[%d], count : %d, available node : %d",
               request->collID, request->reqCount, nodeCount);

        size_t curReqCount = std::min(nodeCount, request->reqCount);

        for (size_t i = 0; i < curReqCount; i++) {

            auto nodeIdle = nodeManager->getIdle(coll);

            nodes.emplace_back(nodeIdle);
        }

        if (request->reqCount > nodeCount) {

            collectorManager->updateRequest(request->reqCount - nodeCount);

        } else {

            collectorManager->removeRequest();
        }

        send2CollectorNodeMsg(coll, nodes);
    }
}

bool Distributor::processBusyDead(size_t busyDeadCount, size_t nodeCount) {

    LOGS_W(getHost(), "Unresponsive node detected, count : %d", busyDeadCount);

    TypeComponentListMapIDList replaceIdList;

    auto processDeadCount = std::min(nodeCount, busyDeadCount);

    for (size_t i = 0; i < processDeadCount; i ++) {

        auto nodeBusy = nodeManager->getBusyDead();

        if (!nodeBusy->getAssigned()) {

            LOGS_W(getHost(), "Unresponsive Node[%d] is not assigned with any Collector", nodeBusy->getID());
            continue;
        }

        auto nodeIdle = nodeManager->getIdle(nodeBusy->getAssigned());

        replaceIdList[nodeBusy->getAssigned()->getID()].emplace_back(nodeBusy);
        replaceIdList[nodeBusy->getAssigned()->getID()].emplace_back(nodeIdle);

        LOGS_W(getHost(), "Node[%d] is replaced with Node[%d] for Collector[%d]",
               nodeBusy->getID(), nodeIdle->getID(), nodeBusy->getAssigned()->getID());
    }

    for (auto &collNodeDeadList : replaceIdList) {

        send2CollectorReplaceMsg(collectorManager->get(collNodeDeadList.first), collNodeDeadList.second);
    }

    return true;
}

CollectorManager *Distributor::getCollectors() const {

    return collectorManager;
}

NodeManager *Distributor::getNodes() const {

    return nodeManager;
}

bool Distributor::processCollectorAliveMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    bool alreadyAdded = false;
    long collID = collectorManager->add(owner->getArch(), owner->getAddress(), alreadyAdded);
    if (alreadyAdded) {
        LOGC_D(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d] is already added", collID);
        notifyUI(NOTIFYTYPE_ONCE);
        return true;
    }

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE,
           "New Collector at address : %s with web : %s is added to network with ID : %d",
           UtilNet::getIPPortString(owner->getAddress()->get()).c_str(),
           UtilNet::getIPPortString(owner->getAddress()->getUI()).c_str(), collID);

    return send2CollectorIDMsg(owner, collID);
}

bool Distributor::processCollectorIDMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    collectorManager->setState(owner->getID(), COLLSTATE_IDLE);

    notifyUI(NOTIFYTYPE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d] is confirmed", owner->getID());

    return true;
}

bool Distributor::processCollectorNodeMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    LOGS_D(getHost(), "Collector[%d] make request, count : %d", owner->getID(), msg->getData().getProcessCount());

    collectorManager->addRequest(owner->getID(), msg->getData().getProcessCount());

    notifyUI(NOTIFYTYPE_ONCE);

    return true;
}

bool Distributor::processCollectorReadyMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    collectorManager->setState(owner->getID(), COLLSTATE_IDLE);

    notifyUI(NOTIFYTYPE_ONCE);

    return true;
}

bool Distributor::processNodeAliveMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    bool alreadyAdded = false;
    long nodeID = nodeManager->add(owner->getArch(), owner->getAddress(), alreadyAdded);
    if (alreadyAdded) {
        LOGC_D(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is already added", nodeID);
        notifyUI(NOTIFYTYPE_ONCE);
        return true;
    }

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE,
           "New Node at address : %s with web : %s is added to network with ID : %d",
           UtilNet::getIPPortString(owner->getAddress()->get()).c_str(),
           UtilNet::getIPPortString(owner->getAddress()->getUI()).c_str(), nodeID);

    return send2NodeIDMsg(owner, nodeID);
}

bool Distributor::processNodeIDMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    nodeManager->setState(owner->getID(), NODESTATE_IDLE);

    notifyUI(NOTIFYTYPE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is confirmed", owner->getID());

    return true;
}

bool Distributor::processNodeBusyMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    if (!nodeManager->get(owner->getID())) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "Could not found a node with id : %ld", owner->getID());
        return false;
    }

    nodeManager->setState(owner->getID(), NODESTATE_BUSY);

    notifyUI(NOTIFYTYPE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is Busy with Collector[%d]\'s process",
           owner->getID(), msg->getData().getID());

    return send2NodeProcessMsg(owner);
}

bool Distributor::processNodeReadyMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    if (!nodeManager->get(owner->getID())) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "Could not found a node with id : %ld", owner->getID());
        return false;
    }

    nodeManager->setState(owner->getID(), NODESTATE_IDLE);

    nodeManager->releaseAssigned(owner->getID());

    notifyUI(NOTIFYTYPE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is IDLE", owner->getID());

    return true;
}

bool Distributor::send2CollectorWakeupMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_WAKEUP);

    return send(target, std::move(msg));
}

bool Distributor::send2CollectorIDMsg(const TypeComponentUnit& target, TypeID id) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_ID, STREAM_ID);

    msg->getData().setID(id);

    return send(target, std::move(msg));
}

bool Distributor::send2CollectorNodeMsg(const TypeComponentUnit& target, TypeComponentUnitList& nodes) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_NODE, STREAM_COMPONENTS);

    msg->getData().setComponentList(nodes);

    return send(target, std::move(msg));
}

bool Distributor::send2CollectorReplaceMsg(const TypeComponentUnit& target, TypeComponentUnitList& nodes) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_REPLACE, STREAM_COMPONENTS);

    msg->getData().setComponentList(nodes);

    return send(target, std::move(msg));
}

bool Distributor::send2NodeWakeupMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_WAKEUP);

    return send(target, std::move(msg));
}

bool Distributor::send2NodeIDMsg(const TypeComponentUnit& target, TypeID id) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_ID, STREAM_ID);

    msg->getData().setID(id);

    return send(target, std::move(msg));
}

bool Distributor::send2NodeProcessMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_PROCESS);

    return send(target, std::move(msg));
}

bool Distributor::sendWakeupMessage(COMPONENT targetType) {

    if (isSupportMulticast(targetType)) {

        auto target = std::make_shared<ComponentUnit>(targetType, getInterfaceMulticastAddress(targetType));

        auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_WAKEUP);

        send(target, std::move(msg));

    } else {

        TypeAddressList list = getInterfaceAddressList(targetType);

        for (auto &address : list) {

            auto target = std::make_shared<ComponentUnit>(targetType, address);

            auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_WAKEUP);

            send(target, std::move(msg));
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
