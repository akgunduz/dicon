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

    TypeTime curTime = time(nullptr);

    std::unique_lock<std::mutex> lock(mutex);

    for (auto& component : components) {

        auto timeDiff = curTime - component.second->getCheckTime();

        //        LOGS_E(host, "%s[%d] time %ld, now : %ld, diff : %ld",
//               ComponentType::getName(iterator->second->getType()),
//               iterator->second->getID(),
//               iterator->second->getCheckTime(),
//               curTime,
//               timeDiff);

        if (timeDiff > ALIVE_INTERVAL) {

            LOGS_W(host, "%s[%d] is removed from network, unresponsive time : %ld",
                   ComponentType::getName(component.second->getType()),
                   component.second->getID(),
                   timeDiff);

            deadList.push_back(component.second->getID());
        }
    }
}

void ComponentManager::updateDead() {

    std::unique_lock<std::mutex> lock(mutex);

    if (!deadList.empty()) {

        for(auto id : deadList) {

            componentsDead[id] = std::move(components[id]);

            components.erase(id);
        }

        deadList.clear();
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

TypeComponentMapIDList& ComponentManager::get(bool is_dead) {

    if (!is_dead) {

        return components;

    } else {

        return componentsDead;
    }
}

TypeComponentUnit ComponentManager::get(TypeID id) {

    auto search = components.find(id);

    if (search != components.end()) {

        return components[id];
    }

    return nullptr;
}

TypeID ComponentManager::add(ARCH arch, TypeAddress& address, bool& isAlreadyAdded) {

    isAlreadyAdded = false;

    for (auto &component : components) {

        if (*component.second->getAddress() != *address) {
            continue;
        }

        isAlreadyAdded = true;

        component.second->setCheckTime();

        return component.second->getID();

    }

    long newID = idCounter++;

    auto object = createUnit(arch, newID, address);

    object->setCheckTime();

    LOGS_I(host, "%s[%d] is added to network",
           ComponentType::getName(object->getType()), object->getID());

    components[newID] = std::move(object);

    return newID;
}

void ComponentManager::clear() {

    components.clear();
    componentsDead.clear();
}
