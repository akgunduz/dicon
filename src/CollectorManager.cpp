//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"
#include "AddressHelper.h"
#include "ComponentManager.h"
#include "CollectorObject.h"


CollectorManager::CollectorManager() {

}

size_t CollectorManager::getWaitingCount() {

    return waitingList.size();
}

TypeWaitingCollector CollectorManager::getWaiting() {

    mutex.lock();

    if (waitingList.empty()) {

        mutex.unlock();

        return 0;
    }

    TypeWaitingCollector collector = waitingList.front();
    waitingList.pop_front();

    mutex.unlock();

    return collector;
}

bool CollectorManager::addWaiting(long address) {

    mutex.lock();

    waitingList.emplace_back(address);

    mutex.unlock();

    return true;
}

void CollectorManager::clearWaiting() {

    mutex.lock();

    waitingList.clear();

    mutex.unlock();
}

CollectorManager::~CollectorManager() {

    clearWaiting();
}

void CollectorManager::setObject(int id, long address) {

    components[address] = new CollectorObject(id, address);
}
