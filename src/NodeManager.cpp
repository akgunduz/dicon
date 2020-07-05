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

ComponentObject* NodeManager::createObject(int id, long address) {

    new NodeObject(id, address);
}
