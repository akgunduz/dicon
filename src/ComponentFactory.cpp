//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#include "ComponentFactory.h"

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

    interface[0] = _interfaces[0];
    interface[1] = _interfaces[1];
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
        components[COMP_DISTRIBUTOR].emplace_back(std::make_unique<Distributor>(interface[0], interface[1], autoWake));
    }

    return true;
}

bool ComponentFactory::startCollector(int count) {

    for (int i = 0; i < count; i++) {

        components[COMP_COLLECTOR].emplace_back(std::make_unique<Collector>(interface[0], interface[1]));
    }

    return true;
}

TypeCollector& ComponentFactory::startCollector() {

    components[COMP_COLLECTOR].emplace_back(std::make_unique<Collector>(interface[0], interface[1]));

    return reinterpret_cast<TypeCollector &>(components[COMP_COLLECTOR].back());
}

bool ComponentFactory::startNode(int count) {

    for (int i = 0; i < count; i++) {

        components[COMP_NODE].emplace_back(std::make_unique<Node>(interface[1]));
    }

    return true;
}

TypeNode& ComponentFactory::startNode() {

    components[COMP_NODE].emplace_back(std::make_unique<Node>(interface[1]));

    return reinterpret_cast<TypeNode &>(components[COMP_NODE].back());
}