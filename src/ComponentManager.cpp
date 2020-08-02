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

ComponentUnit* ComponentManager::get(long id) {

    ComponentUnit *object = nullptr;

    mutex.lock();

    auto search = componentsMapID.find(id);
    if (search != componentsMapID.end()) {

        object = componentsMapID[id];
    }

    mutex.unlock();

    return object;
}

ComponentUnit *ComponentManager::getByIndex(int index) {

    ComponentUnit *object = nullptr;

    mutex.lock();

    if (index < componentsMapID.size()) {

        object = componentsIndex[index];
    }

    mutex.unlock();

    return object;
}

long ComponentManager::add(Address& address, bool& isAlreadyAdded) {

    long newID = 0;

    mutex.lock();

    isAlreadyAdded = false;

    auto search = componentsMapAddress.find(address);
    if (search == componentsMapAddress.end()) {

        newID = idCounter++;
        ComponentUnit *object = createUnit(newID, address);
        componentsMapAddress[address] = object;
        componentsMapID[newID] = object;
        componentsIndex.emplace_back(object);

    } else {

        isAlreadyAdded = true;
        newID = search->second->getID();
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

bool ComponentManager::isExist(long id) {

    return get(id) != nullptr;
}


