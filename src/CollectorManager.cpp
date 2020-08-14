//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"
#include "Address.h"
#include "ComponentManager.h"
#include "CollectorUnit.h"

CollectorManager::CollectorManager(HostUnit *_host, bool autoWake)
    : ComponentManager(_host, autoWake) {
};

CollectorManager::~CollectorManager() = default;

COLLSTATES CollectorManager::getState(long id) {

    COLLSTATES state = COLLSTATE_MAX;

    collMutex.lock();

    auto *coll = (CollectorObject*)get(id);
    if (coll) {
        state = coll->getState();
    }

    collMutex.unlock();

    return state;
}

void CollectorManager::setState(long id, COLLSTATES state) {

    collMutex.lock();

    auto *coll = (CollectorObject*)get(id);
    if (coll) {
        coll->setState(state);
    }

    collMutex.unlock();
}

ComponentUnit *CollectorManager::createUnit(long id, Address& address) {

    return new CollectorUnit(id, address);
}

bool CollectorManager::addRequest(long collID, size_t reqNodeCount) {

    collMutex.lock();

    auto *coll = (CollectorObject*)get(collID);
    if (coll) {

        coll->setState(COLLSTATE_BUSY);

        collReqList.emplace_back(CollectorRequest(collID, reqNodeCount));
    }

    collMutex.unlock();

    return true;
}

CollectorRequest* CollectorManager::getRequest() {

    CollectorRequest* request = nullptr;

    collMutex.lock();

    if (!collReqList.empty()) {

        request = &collReqList.front();
    }

    collMutex.unlock();

    return request;
}

void CollectorManager::updateRequest(size_t reqCount) {

    collMutex.lock();

    collReqList.front().reqCount = reqCount;

    collMutex.unlock();
}

void CollectorManager::removeRequest() {

    collMutex.lock();

    collReqList.pop_front();

    collMutex.unlock();
}


