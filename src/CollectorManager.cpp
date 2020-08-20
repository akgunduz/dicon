//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"
#include "Address.h"
#include "ComponentManager.h"
#include "CollectorUnit.h"

CollectorManager::CollectorManager(HostUnit *_host, bool _protect)
    : ComponentManager(_host, _protect) {
};

CollectorManager::~CollectorManager() = default;

COLLSTATES CollectorManager::getState(long id) {

    std::unique_lock<std::mutex> lock(collMutex);

    auto coll = std::static_pointer_cast<CollectorUnit>(get(id));

    if (coll) {

        return coll->getState();
    }

    return COLLSTATE_MAX;
}

void CollectorManager::setState(long id, COLLSTATES state) {

    std::unique_lock<std::mutex> lock(collMutex);

    auto coll = std::static_pointer_cast<CollectorUnit>(get(id));

    if (coll) {

        coll->setState(state);
    }
}

TypeComponentUnit CollectorManager::createUnit(ARCH arch, long id, Address& address) {

    return std::make_shared<CollectorUnit>(arch, id, address);
}

bool CollectorManager::addRequest(long id, size_t reqNodeCount) {

    std::unique_lock<std::mutex> lock(collMutex);

    auto coll = std::static_pointer_cast<CollectorUnit>(get(id));

    if (coll) {

        coll->setState(COLLSTATE_BUSY);

        collReqList.emplace_back(CollectorRequest(id, reqNodeCount));
    }

    return true;
}

CollectorRequest* CollectorManager::getRequest() {

    CollectorRequest* request = nullptr;

    std::unique_lock<std::mutex> lock(collMutex);

    if (!collReqList.empty()) {

        request = &collReqList.front();
    }

    return request;
}

void CollectorManager::updateRequest(size_t reqCount) {

    std::unique_lock<std::mutex> lock(collMutex);

    collReqList.front().reqCount = reqCount;
}

void CollectorManager::removeRequest() {

    std::unique_lock<std::mutex> lock(collMutex);

    collReqList.pop_front();
}


