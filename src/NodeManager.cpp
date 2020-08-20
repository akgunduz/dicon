//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"

NodeManager::NodeManager(HostUnit *_host, bool _protect)
    : ComponentManager(_host, _protect) {
};

NodeManager::~NodeManager() = default;

TypeNodeUnit NodeManager::getIdle(const TypeComponentUnit& assigned) {

    TypeNodeUnit leastUsedNode = nullptr;

    std::unique_lock<std::mutex> lock(nodeMutex);

    for (const auto& nodePair : get()) {

        auto node = std::static_pointer_cast<NodeUnit>(nodePair.second);

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

    assert(leastUsedNode != nullptr);

    return leastUsedNode;
}

size_t NodeManager::getIdleCount() {

    size_t count = 0;

    std::unique_lock<std::mutex> lock(nodeMutex);

    for (auto &nodePair : get()) {

        auto node = std::static_pointer_cast<NodeUnit>(nodePair.second);

        if (node->getState() == NODESTATE_IDLE) {
            count++;
        }
    }

    return count;
}

TypeNodeUnit NodeManager::getBusyDead() {

    std::unique_lock<std::mutex> lock(nodeMutex);

    for (const auto& component : getDead()) {

        auto node = std::static_pointer_cast<NodeUnit>(component);

        if (node->getState() == NODESTATE_DEAD) {
            continue;
        }

        node->setState(NODESTATE_DEAD);

        return node;
    }

    return nullptr;
}

size_t NodeManager::getBusyDeadCount() {

    size_t count = 0;

    std::unique_lock<std::mutex> lock(nodeMutex);

    for (const auto &component : getDead()) {

        auto node = std::static_pointer_cast<NodeUnit>(component);

        if (node->getState() != NODESTATE_DEAD) {
            count++;
        }
    }

    return count;
}

Address& NodeManager::getAddress(long id) {

    std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = get(id);
    if (node) {

        return node->getAddress();
    }

    return Address::invalid;
}

NODESTATES NodeManager::getState(long id) {

    std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = std::static_pointer_cast<NodeUnit>(get(id));
    if (node) {

        return node->getState();
    }

    return NODESTATE_MAX;
}

void NodeManager::setState(long id, NODESTATES state) {

    std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = std::static_pointer_cast<NodeUnit>(get(id));
    if (node) {

        node->setState(state);
    }
}

TypeComponentUnit NodeManager::getAssigned(long id) {

     std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = std::static_pointer_cast<NodeUnit>(get(id));
    if (node) {

        return node->getAssigned();
    }


    return nullptr;
}

void NodeManager::setAssigned(long id, TypeComponentUnit& assigned) {

    std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = std::static_pointer_cast<NodeUnit>(get(id));
    if (node) {

        node->setAssigned(assigned);
    }
}

void NodeManager::setAssigned(long id, ARCH assignedArch, long assignedID, Address& assignedAddress) {

    std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (node) {

        node->setAssigned(COMP_COLLECTOR, assignedArch, assignedID, assignedAddress);
    }
}

void NodeManager::releaseAssigned(long id) {

    std::unique_lock<std::mutex> lock(nodeMutex);

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (node) {

        node->setAssigned(nullptr);
    }
}


TypeComponentUnit NodeManager::createUnit(ARCH arch, long id, Address& address) {

    return std::make_shared<NodeUnit>(arch, id, address);
}
