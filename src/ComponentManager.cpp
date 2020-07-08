//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentManager.h"

ComponentManager::ComponentManager()
        : idCounter(1) {
}

ComponentManager::~ComponentManager() {

    clear();
}

size_t ComponentManager::size() {

    mutex.lock();

    size_t size = componentsMapID.size();

    mutex.unlock();

    return size;
}

ComponentObject* ComponentManager::get(int id) {

    ComponentObject *object = nullptr;

    mutex.lock();

    auto search = componentsMapID.find(id);
    if (search != componentsMapID.end()) {

        object = componentsMapID[id];
    }

    mutex.unlock();

    return object;
}

ComponentObject *ComponentManager::getByIndex(int index) {

    ComponentObject *object = nullptr;

    mutex.lock();

    if (index < componentsMapID.size()) {

        object = componentsIndex[index];
    }

    mutex.unlock();

    return object;
}

int ComponentManager::add(long address) {

    int newID = 0;

    mutex.lock();

    auto search = componentsMapAddress.find(address);
    if (search == componentsMapAddress.end()) {

        newID = idCounter++;
        ComponentObject *object = createObject(newID, address);
        componentsMapAddress[address] = object;
        componentsMapID[newID] = object;
        componentsIndex.emplace_back(object);
    }

    mutex.unlock();

    return newID;
}

void ComponentManager::clear() {

    mutex.lock();

    for (auto & component : componentsMapID) {

        delete component.second;

    }

    componentsMapID.clear();
    componentsMapAddress.clear();
    componentsIndex.clear();

    mutex.unlock();

}

bool ComponentManager::isExist(int id) {

    return get(id) != nullptr;
}


