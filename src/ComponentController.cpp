//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#include "ComponentController.h"

ComponentController *ComponentController::instance = nullptr;

ComponentController *ComponentController::newInstance(int *interfaces) {

    if (instance) {
        return instance;
    }

    instance = new ComponentController(interfaces);
    return instance;
}

ComponentController::~ComponentController() {

    delete distributor;

    for (int i = 0; i < collectors.size(); i++) {
        delete collectors[i];
    }

    for (int i = 0; i < nodes.size(); i++) {
        delete nodes[i];
    }

    instance = nullptr;
}

ComponentController::ComponentController(int *_interfaces) {

    interface[0] = _interfaces[0];
    interface[1] = _interfaces[1];
}

Distributor *ComponentController::getDistributor() {

    return distributor;
}

long ComponentController::getCollectorCount() {

    return collectors.size();
}

Collector *ComponentController::getCollector(long id) {

    for (auto & collector : collectors) {
        if (collector->getHost().getID() == id) {
            return collector;
        }
    }

    return nullptr;
}

std::vector<Collector *> &ComponentController::getCollectors() {

    return collectors;
}

long ComponentController::getNodeCount() {

    return nodes.size();
}

Node *ComponentController::getNode(long id) {

    for (auto & node : nodes) {
        if (node->getHost().getID() == id) {
            return node;
        }
    }

    return nullptr;
}

std::vector<Node *> &ComponentController::getNodes() {

    return nodes;
}

bool ComponentController::startDistributor(bool autoWake) {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(nullptr, 0), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    distributor = Distributor::newInstance(path, interface[0], interface[1], autoWake);

    return true;
}

bool ComponentController::startCollector(int count) {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(nullptr, 0), COLLECTOR_PATH);
    mkdir(path, 0777);

    for (int i = 0; i < count; i++) {

        collectors.push_back(Collector::newInstance(path, interface[0], interface[1]));
    }

    return true;
}

Collector* ComponentController::startCollector() {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(nullptr, 0), COLLECTOR_PATH);
    mkdir(path, 0777);

    auto *collector = Collector::newInstance(path, interface[0], interface[1]);

    collectors.push_back(collector);

    return collector;
}

bool ComponentController::startNode(int count) {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(nullptr, 0), NODE_PATH);
    mkdir(path, 0777);

    for (int i = 0; i < count; i++) {

        nodes.push_back(Node::newInstance(path, interface[1]));
    }

    return true;
}

Node* ComponentController::startNode() {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(nullptr, 0), NODE_PATH);
    mkdir(path, 0777);

    auto *node = Node::newInstance(path, interface[1]);

    nodes.push_back(node);

    return node;
}
