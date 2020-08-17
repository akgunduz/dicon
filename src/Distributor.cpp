//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DistributorHost.h"
#include "NetUtil.h"
#include "CollectorUnit.h"

Distributor *Distributor::instance = nullptr;

Distributor *Distributor::newInstance(const char* path, int interfaceOther, int interfaceNode, bool autoWake) {

    if (!instance) {
        instance = new Distributor(path, interfaceOther, interfaceNode, autoWake);
    }

    return instance;
}

Distributor::Distributor(const char *rootPath, int interfaceOther, int interfaceNode, bool autoWake) :
        Component(rootPath) {

    host = new DistributorHost(getRootPath());

    addProcessHandler(COMP_COLLECTOR, MSGTYPE_ALIVE, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorIDMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_NODE, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorReadyMsg));

    addProcessHandler(COMP_NODE, MSGTYPE_ALIVE, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeIDMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_BUSY, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg));

    nodeManager = new NodeManager(host, autoWake);

    collectorManager = new CollectorManager(host, autoWake);

    collThread = std::thread([](Distributor *distributor){
        distributor->collProcess();
    }, this);

    if (!autoWake) {
        runPollThread = false;
    }

    pollThread = std::thread([](Distributor *distributor){
            distributor->pollProcess();
        }, this);

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

void Distributor::pollProcess() {

    int loop = 0;

    while(runPollThread) {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (!runPollThread) {
            break;
        }

        if (loop++ < WAKEUP_INTERVAL) {
            continue;
        }

        loop = 0;
        sendWakeupMessagesAll(false);
    }
}

void Distributor::collProcess() {

    while(runCollThread) {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto nodeCount = nodeManager->getIdleCount();
        if (nodeCount == 0) {
            continue;
        }

        size_t busyDeadCount = nodeManager->getBusyDeadCount();

        if (busyDeadCount) {

            LOGS_W(getHost(), "Unresponsive Busy node detected, count : %d", busyDeadCount);

            TypeComponentReplaceIDList replaceIdList;

            size_t processDeadCount = std::min(nodeCount, busyDeadCount);

            for (size_t i = 0; i < processDeadCount; i ++) {

                auto *nodeBusy = nodeManager->getBusyDead();

                long busyNodeAssignedColl = nodeBusy->getAssigned().getID();

                if (busyNodeAssignedColl == 0) {

                    LOGS_W(getHost(), "Unresponsive Node[%d] is not assigned with any Collector", nodeBusy->getID());
                    continue;
                }

                auto *nodeIdle = nodeManager->getIdle(&nodeBusy->getAssigned());

                replaceIdList[busyNodeAssignedColl].emplace_back(nodeBusy);
                replaceIdList[busyNodeAssignedColl].emplace_back(nodeIdle);

                LOGS_W(getHost(), "Node[%d] is replaced with Node[%d] for Collector[%d]",
                       nodeBusy->getID(), nodeIdle->getID(), nodeBusy->getAssigned().getID());
            }

            for (auto &collNodeDeadList : replaceIdList) {
                send2CollectorReplaceMsg(*collectorManager->get(collNodeDeadList.first), collNodeDeadList.second);
            }

            nodeCount = nodeManager->getIdleCount();
            if (nodeCount == 0) {
                continue;
            }
        }

        CollectorRequest *request = collectorManager->getRequest();
        if (request == nullptr) {
            continue;
        }

        TypeComponentList nodes;

        auto *collector = (CollectorUnit *) collectorManager->get(request->collID);

        size_t curReqCount = std::min(nodeCount, request->reqCount);

        for (size_t i = 0; i < curReqCount; i++) {

            auto *nodeIdle = nodeManager->getIdle(collector);

            nodes.emplace_back(nodeIdle);
        }

        if (request->reqCount > nodeCount) {

            collectorManager->updateRequest(request->reqCount - nodeCount);

        } else {

            collectorManager->removeRequest();
        }

        send2CollectorNodeMsg(*collector, nodes);
    }
}

CollectorManager *Distributor::getCollectors() const {
    return collectorManager;
}

NodeManager *Distributor::getNodes() const {
    return nodeManager;
}

bool Distributor::processCollectorAliveMsg(ComponentUnit& owner, Message *msg) {

    bool alreadyAdded = false;
    long collID = collectorManager->add(owner.getArch(), owner.getAddress(), alreadyAdded);
    if (alreadyAdded) {
        LOGC_D(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d] is already added", collID);
        notifyUI(NOTIFYSTATE_ONCE);
        return true;
    }

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE,
           "New Collector at address : %s with web : %s is added to network with ID : %d",
           NetUtil::getIPPortString(owner.getAddress().get()).c_str(),
           NetUtil::getIPPortString(owner.getAddress().getUI()).c_str(), collID);

    return send2CollectorIDMsg(owner, collID);
}

bool Distributor::processCollectorIDMsg(ComponentUnit& owner, Message *msg) {

    collectorManager->setState(owner.getID(), COLLSTATE_IDLE);

    notifyUI(NOTIFYSTATE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d] is confirmed", owner.getID());

    return true;
}

bool Distributor::processCollectorNodeMsg(ComponentUnit& owner, Message *msg) {

    collectorManager->addRequest(owner.getID(), (int)msg->getHeader().getVariant(0));

    notifyUI(NOTIFYSTATE_ONCE);

    return true;
}

bool Distributor::processCollectorReadyMsg(ComponentUnit& owner, Message *msg) {

    collectorManager->setState(owner.getID(), COLLSTATE_IDLE);

    notifyUI(NOTIFYSTATE_ONCE);

    return true;
}

bool Distributor::processNodeAliveMsg(ComponentUnit& owner, Message *msg) {

    bool alreadyAdded = false;
    long nodeID = nodeManager->add(owner.getArch(), owner.getAddress(), alreadyAdded);
    if (alreadyAdded) {
        LOGC_D(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is already added", nodeID);
        notifyUI(NOTIFYSTATE_ONCE);
        return true;
    }

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE,
           "New Node at address : %s with web : %s is added to network with ID : %d",
           NetUtil::getIPPortString(owner.getAddress().get()).c_str(),
           NetUtil::getIPPortString(owner.getAddress().getUI()).c_str(), nodeID);

    return send2NodeIDMsg(owner, nodeID);
}

bool Distributor::processNodeIDMsg(ComponentUnit& owner, Message *msg) {

    nodeManager->setState(owner.getID(), NODESTATE_IDLE);

    notifyUI(NOTIFYSTATE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is confirmed", owner.getID());

    return true;
}

bool Distributor::processNodeBusyMsg(ComponentUnit& owner, Message *msg) {

    if (!nodeManager->isExist(owner.getID())) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "Could not found a node with id : %ld", owner.getID());
        return false;
    }

    int collID = (int) msg->getHeader().getVariant(0);

    nodeManager->setState(owner.getID(), NODESTATE_BUSY);

    notifyUI(NOTIFYSTATE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is Busy with Collector[%d]\'s process", owner.getID(), collID);

    return send2NodeProcessMsg(owner);
}

bool Distributor::processNodeReadyMsg(ComponentUnit& owner, Message *msg) {

    if (!nodeManager->isExist(owner.getID())) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "Could not found a node with id : %ld", owner.getID());
        return false;
    }

    nodeManager->setState(owner.getID(), NODESTATE_IDLE);

    nodeManager->releaseAssigned(owner.getID());

    notifyUI(NOTIFYSTATE_ONCE);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] is IDLE", owner.getID());

    return true;
}

bool Distributor::send2CollectorWakeupMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_WAKEUP);

    return send(target, msg);
}

bool Distributor::send2CollectorIDMsg(ComponentUnit& target, long id) {

    auto *msg = new Message(getHost(), target, MSGTYPE_ID);

    msg->getHeader().setVariant(0, id);

    return send(target, msg);
}

bool Distributor::send2CollectorNodeMsg(ComponentUnit& target, TypeComponentList& nodes) {

    auto *msg = new Message(getHost(), target, MSGTYPE_NODE);

    msg->getData().setStreamFlag(STREAM_COMPONENT);
    msg->getData().setComponentList(nodes);

    return send(target, msg);
}

bool Distributor::send2CollectorReplaceMsg(ComponentUnit& target, TypeComponentList& nodes) {

    auto *msg = new Message(getHost(), target, MSGTYPE_REPLACE);

    msg->getData().setStreamFlag(STREAM_COMPONENT);
    msg->getData().setComponentList(nodes);

    return send(target, msg);
}

bool Distributor::send2NodeWakeupMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_WAKEUP);

    return send(target, msg);
}

bool Distributor::send2NodeIDMsg(ComponentUnit& target, long id) {

    auto *msg = new Message(getHost(), target, MSGTYPE_ID);

    msg->getHeader().setVariant(0, id);

    return send(target, msg);
}

bool Distributor::send2NodeProcessMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_PROCESS);

    return send(target, msg);
}

bool Distributor::sendWakeupMessage(COMPONENT targetType) {

    auto target = ComponentUnit(targetType);

    if (isSupportMulticast(targetType)) {

        auto *msg = new Message(getHost(), target, MSGTYPE_WAKEUP);

        target.setAddress(getInterfaceMulticastAddress(targetType), true);
        send(target, msg);

    } else {

        TypeAddressList list = getInterfaceAddressList(targetType);

        for (auto &address : list) {

            auto *msg = new Message(getHost(), target, MSGTYPE_WAKEUP);

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
