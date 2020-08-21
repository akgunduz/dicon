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

    PRINT("Deallocating ComponentController");

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
        if (collector->getHost().getID() == id) {
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
        if (node->getHost().getID() == id) {
            return reinterpret_cast<TypeNode &>(node);
        }
    }

    return reinterpret_cast<TypeNode &>(Component::nullComponent);
}

TypeComponentList& ComponentFactory::getNodes() {

    return components[COMP_NODE];
}

bool ComponentFactory::startDistributor(bool autoWake) {

    char path[PATH_MAX];
    char cwd[PATH_MAX - 100];
    sprintf(path, "%s/%s", getcwd(cwd, PATH_MAX - 100), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    if (components[COMP_DISTRIBUTOR].empty()) {
        components[COMP_DISTRIBUTOR].emplace_back(Distributor::newInstance(path, interface[0], interface[1], autoWake));
    }

    return true;
}

bool ComponentFactory::startCollector(int count) {

    char path[PATH_MAX];
    char cwd[PATH_MAX - 100];
    sprintf(path, "%s/%s", getcwd(cwd, PATH_MAX - 100), COLLECTOR_PATH);
    mkdir(path, 0777);

    for (int i = 0; i < count; i++) {

        components[COMP_COLLECTOR].emplace_back(Collector::newInstance(path, interface[0], interface[1]));
    }

    return true;
}

TypeCollector& ComponentFactory::startCollector() {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(nullptr, 0), COLLECTOR_PATH);
    mkdir(path, 0777);

    components[COMP_COLLECTOR].emplace_back(Collector::newInstance(path, interface[0], interface[1]));

    return reinterpret_cast<TypeCollector &>(components[COMP_COLLECTOR].back());
}

bool ComponentFactory::startNode(int count) {

    char path[PATH_MAX];
    char cwd[PATH_MAX - 100];
    sprintf(path, "%s/%s", getcwd(cwd, PATH_MAX - 100), NODE_PATH);
    mkdir(path, 0777);

    for (int i = 0; i < count; i++) {

        components[COMP_NODE].emplace_back(Node::newInstance(path, interface[1]));
    }

    return true;
}

TypeNode& ComponentFactory::startNode() {

    char path[PATH_MAX];
    char cwd[PATH_MAX - 100];
    sprintf(path, "%s/%s", getcwd(cwd, PATH_MAX - 100), NODE_PATH);
    mkdir(path, 0777);

    components[COMP_NODE].emplace_back(Node::newInstance(path, interface[1]));

    return reinterpret_cast<TypeNode &>(components[COMP_NODE].back());
}
