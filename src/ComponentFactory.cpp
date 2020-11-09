//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "ComponentFactory.h"
#include "Log.h"

ComponentFactory *ComponentFactory::instance = nullptr;

ComponentFactory *ComponentFactory::newInstance(int *interfaces) {

    if (!instance) {
        instance = new ComponentFactory(interfaces);
    }

    return instance;
}

ComponentFactory::~ComponentFactory() {

    LOGP_T("Deallocating ComponentController");

    instance = nullptr;
}

ComponentFactory::ComponentFactory(const int *_interfaces) {

    commInterfaces[0] = _interfaces[0];
    commInterfaces[1] = _interfaces[1];
}

bool ComponentFactory::hasDistributor() {

    return components[COMP_DISTRIBUTOR].size() > 0;
}

TypeDistributor& ComponentFactory::getDistributor() {

    return reinterpret_cast<TypeDistributor &>(components[COMP_DISTRIBUTOR][0]);
}

size_t ComponentFactory::getCollectorCount() {

    return components[COMP_COLLECTOR].size();
}

TypeCollector& ComponentFactory::getCollector(long id) {

    for (auto& collector : components[COMP_COLLECTOR]) {
        if (collector->getHost()->getID() == id) {
            return reinterpret_cast<TypeCollector &>(collector);
        }
    }

    return reinterpret_cast<TypeCollector &>(Component::nullComponent);
}

TypeComponentList& ComponentFactory::getCollectors() {

    return components[COMP_COLLECTOR];
}

size_t ComponentFactory::getNodeCount() {

    return components[COMP_NODE].size();
}

TypeNode& ComponentFactory::getNode(long id) {

    for (auto& node : components[COMP_NODE]) {
        if (node->getHost()->getID() == id) {
            return reinterpret_cast<TypeNode &>(node);
        }
    }

    return reinterpret_cast<TypeNode &>(Component::nullComponent);
}

TypeComponentList& ComponentFactory::getNodes() {

    return components[COMP_NODE];
}

bool ComponentFactory::startDistributor(bool autoWake) {

    if (components[COMP_DISTRIBUTOR].empty()) {
        auto distributor = std::make_unique<Distributor>(commInterfaces[0], commInterfaces[1], autoWake);
        if (!distributor->isInitialized()) {
            return false;
        }
        components[COMP_DISTRIBUTOR].emplace_back(std::move(distributor));
    }

    return true;
}

bool ComponentFactory::startCollector(int count) {

    for (int i = 0; i < count; i++) {

        if (startCollector() == Component::nullComponent) {
            return false;
        }
     }

    return true;
}

TypeCollector& ComponentFactory::startCollector() {

    auto collector = std::make_shared<Collector>(commInterfaces[0], commInterfaces[1]);
    if (!collector->isInitialized()) {
        return reinterpret_cast<TypeCollector &>(Component::nullComponent);
    }

    components[COMP_COLLECTOR].emplace_back(std::move(collector));

    return reinterpret_cast<TypeCollector &>(components[COMP_COLLECTOR].back());
}

bool ComponentFactory::startNode(int count) {

    for (int i = 0; i < count; i++) {

        if (startNode() == Component::nullComponent) {
            return false;
        }
    }

    return true;
}

TypeNode& ComponentFactory::startNode() {

    auto node = std::make_shared<Node>(commInterfaces[1]);
    if (!node->isInitialized()) {
        return reinterpret_cast<TypeNode &>(Component::nullComponent);
    }

    components[COMP_NODE].emplace_back(std::move(node));

    return reinterpret_cast<TypeNode &>(components[COMP_NODE].back());
}
