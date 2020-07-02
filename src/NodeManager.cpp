//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"
#include "AddressHelper.h"

NodeManager::NodeManager() {

};

NodeManager::~NodeManager() {

}

long NodeManager::getIdle() {

    NodeObject* leastUsedNode = NULL;

    mutex.lock();

    for (auto i = components.begin(); i != components.end(); i++) {

        auto *node = (NodeObject*) i->second;

        if (node->getState() != NODESTATE_IDLE) {
            continue;
        }

        if (leastUsedNode == NULL) {
            leastUsedNode = node;
            continue;
        }

        if (node->getUsage() < leastUsedNode->getUsage()) {
            leastUsedNode = node;
        }
    }

    if (leastUsedNode != NULL) {

        leastUsedNode->iterateUsage(true);
        leastUsedNode->setState(NODESTATE_PREBUSY);

        mutex.unlock();

        return leastUsedNode->getAddress();

    }

    mutex.unlock();

    return 0;
}

bool NodeManager::setState(long address, NODESTATES state) {

    auto search = components.find(address);
    if (search == components.end()) {
        return false;
    }

    auto *node = (NodeObject*) search->second;

    mutex.lock();

    node->setState(state);

    mutex.unlock();

    return true;
}

void NodeManager::setObject(int id, long address) {

    components[address] = new NodeObject(id, address);
}
