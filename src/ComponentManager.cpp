//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentManager.h"

ComponentManager::ComponentManager()
        : idCounter(1) {

}

ComponentManager::~ComponentManager() {

    components.clear();
}

size_t ComponentManager::size() {

    return components.size();
}

ComponentObject* ComponentManager::get(long address) {

    auto search = components.find(address);
    if (search == components.end()) {
        return nullptr;
    }

    return components[address];

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

        setObject(getFreeID(), address);

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

ComponentObject *ComponentManager::getByIndex(int index) {

    int j = 0;
    if (index >= size()) {
        return nullptr;
    }

    for (auto & component : components) {

        if (j++ == index) {
            return component.second;
        }
    }

    return nullptr;
}
