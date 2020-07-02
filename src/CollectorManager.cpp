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

CollectorManager::~CollectorManager() {

    clearWaiting();
}

void CollectorManager::setObject(int id, long address) {

    components[address] = new CollectorObject(id, address);
}

//ComponentObject *CollectorManager::getWaiting() {
//
//    for (auto & component : components) {
//
//        auto *collector = (CollectorObject*) component.second;
//
//        if (collector->getState() == COLLSTATE_WAITING) {
//            return collector;
//        }
//    }
//
//    return nullptr;
//}

bool CollectorManager::attachNode(long address, ComponentObject &node) {

    auto search = components.find(address);
    if (search == components.end()) {
        return false;
    }

    auto *collector = (CollectorObject*) search->second;

    mutex.lock();

    collector->setAttached(node);

    mutex.unlock();

    return true;
}

bool CollectorManager::detachNode(long address) {

    auto search = components.find(address);
    if (search == components.end()) {
        return false;
    }

    auto *collector = (CollectorObject*) search->second;

    mutex.lock();

    collector->setAttached(ComponentObject());

    mutex.unlock();

    return true;
}
