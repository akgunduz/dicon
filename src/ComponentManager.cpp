//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentManager.h"
#include "AddressHelper.h"

ComponentManager::ComponentManager()
        : idCounter(1) {

}

ComponentManager::~ComponentManager() {

    components.clear();
}

int ComponentManager::getID(long address) {

    auto search = components.find(address);
    if (search == components.end()) {
        return 0;
    }

    return components[address]->getID();

}

int ComponentManager::add(long address) {

    auto search = components.find(address);
    if (search == components.end()) {

        mutex.lock();

        setObject(address);

        mutex.unlock();

        return true;
    }

    return false;
}


int ComponentManager::getFreeID() {

    return idCounter++;
}

void ComponentManager::clear() {

    mutex.lock();

    for (auto i = components.begin(); i != components.end(); i++) {

        delete i->second;

    }

    components.clear();

    mutex.unlock();

}
