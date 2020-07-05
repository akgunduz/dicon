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

    size_t size = componentsMap.size();

    mutex.unlock();

    return size;
}

ComponentObject* ComponentManager::get(int id) {

    ComponentObject *object = nullptr;

    mutex.lock();

    auto search = componentsMap.find(id);
    if (search != componentsMap.end()) {

        object = componentsMap[id];
    }

    mutex.unlock();

    return object;
}

ComponentObject *ComponentManager::getByIndex(int index) {

    ComponentObject *object = nullptr;

    mutex.lock();

    if (index < componentsMap.size()) {

        object = componentsIndex[index];
    }

    mutex.unlock();

    return object;
}

int ComponentManager::add(long address) {

    int newID = 0;

    mutex.lock();

    auto search = componentsMap.find(address);
    if (search == componentsMap.end()) {

        newID = idCounter++;
        ComponentObject *object = createObject(newID, address);
        componentsMap[newID] = object;
        componentsIndex.emplace_back(object);
    }

    mutex.unlock();

    return newID;
}

void ComponentManager::clear() {

    mutex.lock();

    for (auto & component : componentsMap) {

        delete component.second;

    }

    componentsMap.clear();
    componentsIndex.clear();

    mutex.unlock();

}

bool ComponentManager::isExist(int id) {

    return get(id) != nullptr;
}


