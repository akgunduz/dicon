//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentManager.h"
#include "Log.h"

ComponentManager::ComponentManager(HostUnit *_host, bool autoWake)
        : idCounter(1), host(_host) {

    if (!autoWake) {
        return;
    }

    thread = std::thread([](ComponentManager *manager){
        manager->process();
    }, this);
}

ComponentManager::~ComponentManager() {

    threadRun = false;
    thread.join();
    clear();
}

void ComponentManager::process() {

    int loop = 0;

    while(threadRun) {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (!threadRun) {
            break;
        }

        if (loop++ < CHECK_INTERVAL) {
            continue;
        }

        loop = 0;

        long curTime = time(nullptr);

        mutex.lock();

        for (auto iterator = componentsMapID.begin(); iterator != componentsMapID.end();) {

            if (curTime - iterator->second->getCheckTime() > ALIVE_INTERVAL) {

                ComponentUnit *object = iterator->second;
                componentsMapAddress.erase(object->getAddress());
                iterator = componentsMapID.erase(iterator);

                LOGS_I(*host, "%s[%d] is removed from network",
                      ComponentType::getName(object->getType()), object->getID());

                componentsMapDead.emplace_back(object);

            } else {

                ++iterator;
            }
        }

        mutex.unlock();
    }
}

size_t ComponentManager::size() {

    mutex.lock();

    size_t size = componentsMapID.size();

    mutex.unlock();

    return size;
}

TypeComponentMapIDList &ComponentManager::get() {

    return componentsMapID;
}

TypeComponentList &ComponentManager::getDead() {

    return componentsMapDead;
}

ComponentUnit* ComponentManager::getDead(long index) {

    if (index < componentsMapDead.size()) {
        return componentsMapDead[index];
    }

    return nullptr;
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

long ComponentManager::add(ARCH arch, Address& address, bool& isAlreadyAdded) {

    long newID = 0;

    mutex.lock();

    isAlreadyAdded = false;

    auto search = componentsMapAddress.find(address);
    if (search == componentsMapAddress.end()) {

        newID = idCounter++;

        ComponentUnit *object = createUnit(arch, newID, address);

        object->setCheckTime(time(nullptr));

        componentsMapAddress[address] = object;
        componentsMapID[newID] = object;

        LOGS_I(*host, "%s[%d] is added to network",
               ComponentType::getName(object->getType()), object->getID());

    } else {

        isAlreadyAdded = true;

        search->second->setCheckTime(time(nullptr));

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

    for (auto & component : componentsMapDead) {

        delete component;

    }

    componentsMapID.clear();
    componentsMapDead.clear();
    componentsMapAddress.clear();

    mutex.unlock();

}

bool ComponentManager::isExist(long id) {

    return get(id) != nullptr;
}



