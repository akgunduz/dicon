//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"
#include "AddressHelper.h"

NodeManager::NodeManager() = default;

NodeManager::~NodeManager() = default;

NodeObject NodeManager::getIdle() {

    NodeObject nodeObject;

    NodeObject* leastUsedNode = nullptr;

    nodeMutex.lock();

    for (int i = 0; i < size(); i++) {

        auto *node = (NodeObject*) getByIndex(i);

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

        nodeObject = *leastUsedNode;

    }

    nodeMutex.unlock();

    return nodeObject;
}

int NodeManager::getIdleCount() {

    int count = 0;

    nodeMutex.lock();

    for (int i = 0; i < size(); i++) {

        auto *node = (NodeObject*) getByIndex(i);

        if (node->getState() == NODESTATE_IDLE) {
            count++;
        }
    }

    nodeMutex.unlock();

    return count;
}

long NodeManager::getAddress(int id) {

    long address = 0;

    nodeMutex.lock();

    auto *node = (NodeObject*)get(id);
    if (node) {
        address = node->getAddress();
    }

    nodeMutex.unlock();

    return address;
}

NODESTATES NodeManager::getState(int id) {

    NODESTATES state = NODESTATE_MAX;

    nodeMutex.lock();

    auto *node = (NodeObject*)get(id);
    if (node) {
        state = node->getState();
    }

    nodeMutex.unlock();

    return state;
}

void NodeManager::setState(int id, NODESTATES state) {

    nodeMutex.lock();

    auto *node = (NodeObject*)get(id);
    if (node) {
        node->setState(state);
    }

    nodeMutex.unlock();
}

ComponentInfo NodeManager::getAssigned(int id) {

    ComponentInfo assigned;

    nodeMutex.lock();

    auto *node = (NodeObject*)get(id);
    if (node) {
        assigned = node->getAssigned();
    }

    nodeMutex.unlock();

    return assigned;
}

void NodeManager::setAssigned(int id, ComponentInfo& assigned) {

    nodeMutex.lock();

    auto *node = (NodeObject*)get(id);
    if (node) {
        node->setAssigned(assigned.getID(), assigned.getAddress());
    }

    nodeMutex.unlock();
}

void NodeManager::setAssigned(int id, int assignedID, long assignedAddress) {

    nodeMutex.lock();

    auto *node = (NodeObject*)get(id);
    if (node) {
        node->setAssigned(assignedID, assignedAddress);
    }

    nodeMutex.unlock();
}


ComponentObject* NodeManager::createObject(int id, long address) {

    return new NodeObject(id, address);
}
