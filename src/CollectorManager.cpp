//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"
#include "AddressHelper.h"
#include "ComponentManager.h"
#include "CollectorObject.h"

CollectorManager::CollectorManager() = default;

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

ComponentObject *CollectorManager::createObject(long id, long address) {

    return new CollectorObject(id, address);
}

bool CollectorManager::addRequest(long collID, int reqNodeCount) {

    collMutex.lock();

    collReqList.emplace_back(CollectorRequest(collID, reqNodeCount));

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

void CollectorManager::updateRequest(int reqCount) {

    collMutex.lock();

    collReqList.front().reqCount = reqCount;

    collMutex.unlock();
}

void CollectorManager::removeRequest() {

    collMutex.lock();

    collReqList.pop_front();

    collMutex.unlock();
}


