//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"

NodeManager::NodeManager() {

};

NodeManager::~NodeManager() {

}

long NodeManager::getIdle() {

//TODO coklu collector de burada thread korumasi olmasi lazim

    NodeObject leastUsedNode(IDLE, 0x7FFFFFFF);
    long leastUsedAddress = 0;

    int idleCount = 0;

    for (auto i = nodes.begin(); i != nodes.end(); i++) {
        NodeObject node = i->second;
        if (node.getState() != IDLE) {
            continue;
        }

        idleCount++;

        if (node.getUsage() < leastUsedNode.getUsage()) {
            leastUsedNode = node;
            leastUsedAddress = i->first;
        }
    }

    if (leastUsedAddress != 0) {

        setState(leastUsedAddress, PREBUSY);

        return leastUsedAddress;

    }

    LOG_W("No available node right now.");

    return 0;
}

bool NodeManager::add(long address) {

    auto search = nodes.find(address);
    if (search == nodes.end()) {

        mutex.lock();

        nodes[address] = NodeObject();

        mutex.unlock();

        LOG_I("Node at address : %s added to the list",
              InterfaceTypes::getAddressString(address).c_str());

        return true;
    }

	return true;
}

void NodeManager::clear() {

    mutex.lock();

    nodes.clear();

    mutex.unlock();
}

bool NodeManager::setState(long address, NODE_STATES state) {

    auto search = nodes.find(address);
    if (search == nodes.end()) {
        LOG_I("Could not found a node with address : %s",
              InterfaceTypes::getAddressString(address).c_str());
        return false;
    }

    NodeObject node = search->second;

    mutex.lock();

    switch(state) {

        case IDLE:
            nodes[address] = NodeObject(IDLE, node.getUsage());
            break;

        case PREBUSY:
            nodes[address] = NodeObject(PREBUSY, node.getUsage());
            break;

        case BUSY:
            nodes[address] = NodeObject(BUSY, node.iterateUsage(true));
            break;

        default:
            break;
    }

    mutex.unlock();

    LOG_T("Node at address : %s switch to state : %s",
          InterfaceTypes::getAddressString(address).c_str(), NodeState::getName(BUSY));
    return true;
}
