//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentManager.h"
#include "Log.h"

ComponentManager::ComponentManager(const TypeHostUnit& _host, bool _protect)
        : idCounter(1), host(_host), protect(_protect) {

    if (protect) {
        return;
    }

    thread = std::thread([](ComponentManager *manager){
        manager->process();
    }, this);
}

ComponentManager::~ComponentManager() {

    if (!protect) {
        threadRun = false;
        thread.join();
    }

    clear();
}

void ComponentManager::checkDead() {

    std::unique_lock<std::mutex> lock(mutex);

    long long curTime = time(nullptr);

    for (auto iterator = componentsMapID.begin(); iterator != componentsMapID.end();) {

        if (curTime - iterator->second->getCheckTime() > ALIVE_INTERVAL) {

            LOGS_I(host, "%s[%d] is removed from network",
                   ComponentType::getName(iterator->second->getType()), iterator->second->getID());

            componentsMapDead.emplace_back(std::move(iterator->second));

            iterator = componentsMapID.erase(iterator);

        } else {

            ++iterator;
        }
    }
}

void ComponentManager::process() {

    int loop = 0;

    while(threadRun) {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (loop++ < CHECK_INTERVAL) {
            continue;
        }

        loop = 0;

        checkDead();
    }
}

size_t ComponentManager::size() {

    return componentsMapID.size();
}

TypeComponentMapIDList& ComponentManager::get() {

    return componentsMapID;
}

TypeComponentUnit ComponentManager::get(TypeID id) {

    std::unique_lock<std::mutex> lock(mutex);

    auto search = componentsMapID.find(id);
    if (search != componentsMapID.end()) {

        return componentsMapID[id];
    }

    return nullptr;
}

TypeComponentUnitList& ComponentManager::getDead() {

    return componentsMapDead;
}

TypeComponentUnit ComponentManager::getDead(size_t index) {

    std::unique_lock<std::mutex> lock(mutex);

    if (index < componentsMapDead.size()) {
        return componentsMapDead[index];
    }

    return nullptr;
}

TypeID ComponentManager::add(ARCH arch, Address& address, bool& isAlreadyAdded) {

    std::unique_lock<std::mutex> lock(mutex);

    isAlreadyAdded = false;

    for (auto &component : componentsMapID) {

        if (component.second->getAddress() != address) {
            continue;
        }

        isAlreadyAdded = true;

        component.second->setCheckTime(std::chrono::system_clock::now().time_since_epoch().count());

        return component.second->getID();

    }

    long newID = idCounter++;

    auto object = createUnit(arch, newID, address);

    object->setCheckTime(std::chrono::system_clock::now().time_since_epoch().count());

    LOGS_I(host, "%s[%d] is added to network",
           ComponentType::getName(object->getType()), object->getID());

    componentsMapID[newID] = std::move(object);

    return newID;
}

void ComponentManager::clear() {

    std::unique_lock<std::mutex> lock(mutex);

    componentsMapID.clear();
    componentsMapDead.clear();
}
