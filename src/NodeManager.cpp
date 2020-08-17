//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"
#include "Address.h"

NodeManager::NodeManager(HostUnit *_host, bool autoWake)
    : ComponentManager(_host, autoWake) {
};

NodeManager::~NodeManager() = default;

NodeUnit* NodeManager::getIdle(ComponentUnit* assigned) {

    NodeUnit* leastUsedNode = nullptr;

    nodeMutex.lock();

    for (auto &nodePair : get()) {

        auto *node = (NodeUnit*)nodePair.second;

        if (node->getState() != NODESTATE_IDLE) {
            continue;
        }

        if (leastUsedNode == nullptr) {
            leastUsedNode = node;
            continue;
        }

        if (node->getUsage() < leastUsedNode->getUsage()) {
            leastUsedNode = node;
        }
    }

    if (leastUsedNode != nullptr) {

        leastUsedNode->iterateUsage(true);
        leastUsedNode->setState(NODESTATE_PREBUSY);
        leastUsedNode->setAssigned(assigned);
    }

    nodeMutex.unlock();

    assert(leastUsedNode != nullptr);

    return leastUsedNode;
}

size_t NodeManager::getIdleCount() {

    size_t count = 0;

    nodeMutex.lock();

    for (auto &nodePair : get()) {

        auto *node = (NodeUnit*) nodePair.second;

        if (node->getState() == NODESTATE_IDLE) {
            count++;
        }
    }

    nodeMutex.unlock();

    return count;
}

NodeUnit* NodeManager::getBusyDead() {

    NodeUnit *busyDeadNode = nullptr;

    nodeMutex.lock();

    for (auto *component : getDead()) {

        auto *node = (NodeUnit*)component;

        if (node->getState() == NODESTATE_DEAD) {
            continue;
        }

        node->setState(NODESTATE_DEAD);

        busyDeadNode = node;

        nodeMutex.unlock();

        return busyDeadNode;
    }

    nodeMutex.unlock();

    assert(busyDeadNode != nullptr);

    return busyDeadNode;
}

size_t NodeManager::getBusyDeadCount() {

    size_t count = 0;

    nodeMutex.lock();

    for (auto *component : getDead()) {

        auto *node = (NodeUnit*)component;

        if (node->getState() != NODESTATE_DEAD) {
            count++;
        }
    }

    nodeMutex.unlock();

    return count;
}

Address& NodeManager::getAddress(long id) {

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {

        Address &address = node->getAddress();
        nodeMutex.unlock();
        return address;
    }

    nodeMutex.unlock();

    return Address::invalid;
}

NODESTATES NodeManager::getState(long id) {

    NODESTATES state = NODESTATE_MAX;

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {
        state = node->getState();
    }

    nodeMutex.unlock();

    return state;
}

void NodeManager::setState(long id, NODESTATES state) {

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {
        node->setState(state);
    }

    nodeMutex.unlock();
}

ComponentUnit NodeManager::getAssigned(long id) {

    ComponentUnit assigned;

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {
        assigned = node->getAssigned();
    }

    nodeMutex.unlock();

    return assigned;
}

void NodeManager::setAssigned(long id, ComponentUnit& assigned) {

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {
        node->setAssigned(assigned.getType(), assigned.getID(), assigned.getAddress());
    }

    nodeMutex.unlock();
}

void NodeManager::setAssigned(long id, long assignedID, Address& assignedAddress) {

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {
        node->setAssigned(COMP_COLLECTOR, assignedID, assignedAddress);
    }

    nodeMutex.unlock();
}

void NodeManager::releaseAssigned(long id) {

    nodeMutex.lock();

    auto *node = (NodeUnit*)get(id);
    if (node) {
        node->setAssigned(COMP_COLLECTOR, 0, Address());
    }

    nodeMutex.unlock();
}


ComponentUnit* NodeManager::createUnit(ARCH arch, long id, Address& address) {

    return new NodeUnit(arch, id, address);
}
