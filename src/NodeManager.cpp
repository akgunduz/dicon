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

    TypeComponentList::iterator leastUsedNode = components.end();

    mutex.lock();

    for (auto i = components.begin(); i != components.end(); i++) {

        auto *node = (NodeObject*) i->second;

        if (node->getState() != IDLE) {
            continue;
        }

        if (leastUsedNode == components.end()) {
            leastUsedNode = i;
            continue;
        }

        if (node->getUsage() < ((NodeObject*)leastUsedNode->second)->getUsage()) {
            leastUsedNode = i;
        }
    }

    if (leastUsedNode != components.end()) {

        ((NodeObject*)leastUsedNode->second)->setState(PREBUSY);

        mutex.unlock();

        return leastUsedNode->first;

    }

    mutex.unlock();

    return 0;
}

bool NodeManager::setState(long address, NODE_STATES state) {

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

void NodeManager::setObject(long address) {

    components[address] = new NodeObject(AddressHelper::getID(address));
}
