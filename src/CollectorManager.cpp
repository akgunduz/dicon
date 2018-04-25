//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"

size_t CollectorManager::getCount() {

    return waitingList.size();
}

TypeWaitingCollector CollectorManager::get() {

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

bool CollectorManager::add(long address, const char *jobDir) {

    mutex.lock();

    waitingList.emplace_back(TypeWaitingCollector(address, jobDir));

    mutex.unlock();

    return true;
}

void CollectorManager::clear() {

    mutex.lock();

    waitingList.clear();

    mutex.unlock();
}

