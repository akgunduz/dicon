//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"
#include "AddressHelper.h"

int CollectorManager::idCounter = 1;

size_t CollectorManager::getWaitingCount() {

    return waitingList.size();
}

TypeWaitingCollector CollectorManager::getWaiting() {

    mutex.lock();

    if (waitingList.empty()) {

        mutex.unlock();

        return TypeWaitingCollector(0, "");
    }

    TypeWaitingCollector collector = waitingList.front();
    waitingList.pop_front();

    mutex.unlock();

    return collector;
}

bool CollectorManager::addWaiting(long address, std::string jobDir) {

    mutex.lock();

    waitingList.emplace_back(TypeWaitingCollector(address, jobDir));

    mutex.unlock();

    return true;
}

void CollectorManager::clearWaiting() {

    mutex.lock();

    waitingList.clear();

    mutex.unlock();
}

int CollectorManager::add(long address) {

    auto search = collectors.find(address);
    if (search == collectors.end()) {

        mutex.lock();

        collectors[address] = AddressHelper::getID(address);

        mutex.unlock();

        return true;
    }

    return false;
}

int CollectorManager::getID(long address) {

    auto search = collectors.find(address);
    if (search == collectors.end()) {
        return 0;
    }

    return collectors[address];
}

void CollectorManager::clear() {

    clearWaiting();
    collectors.clear();
}

CollectorManager::~CollectorManager() {

    clearWaiting();
    collectors.clear();
}

int CollectorManager::getFreeID() {

    return idCounter++;
}

