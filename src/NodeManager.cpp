//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"
#include "Address.h"

NodeManager::NodeManager() = default;

NodeManager::~NodeManager() = default;

NodeUnit NodeManager::getIdle() {

    NodeUnit nodeUnit{};

    NodeUnit* leastUsedNode = nullptr;

    nodeMutex.lock();

    for (int i = 0; i < size(); i++) {

        auto *node = (NodeUnit*) getByIndex(i);

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

        nodeUnit = *leastUsedNode;

    }

    nodeMutex.unlock();

    return nodeUnit;
}

int NodeManager::getIdleCount() {

    int count = 0;

    nodeMutex.lock();

    for (int i = 0; i < size(); i++) {

        auto *node = (NodeUnit*) getByIndex(i);

        if (node->getState() == NODESTATE_IDLE) {
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


ComponentUnit* NodeManager::createUnit(long id, Address& address) {

    return new NodeUnit(id, address);
}
