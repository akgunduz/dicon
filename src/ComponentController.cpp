//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#include "ComponentController.h"

ComponentController *ComponentController::instance = nullptr;

ComponentController *ComponentController::newInstance(int ind1, int ind2) {

    if (instance) {
        return instance;
    }

    instance = new ComponentController(ind1, ind2);
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

    instance = NULL;
}

ComponentController::ComponentController(int ind1, int ind2) {

    setInterfaces(ind1, ind2);
}

Distributor *ComponentController::getDistributor() {

    return distributor;
}

long ComponentController::getCollectorCount() {

    return collectors.size();
}

Collector *ComponentController::getCollector(int id) {

    for (auto & collector : collectors) {
        if (collector->getHost().getID() == id) {
            return collector;
        }
    }

    return nullptr;
}

long ComponentController::getNodeCount() {

    return nodes.size();
}

Node *ComponentController::getNode(int id) {

    for (auto & node : nodes) {
        if (node->getHost().getID() == id) {
            return node;
        }
    }

    return nullptr;
}

bool ComponentController::startDistributor() {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(NULL, 0), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    distributor = Distributor::newInstance(path);

    return true;
}

bool ComponentController::startCollector(int count, int startIndex) {

    for (int i = 0; i < count; i++) {

        char path[PATH_MAX];
        sprintf(path, "%s/%s_%ld", getcwd(NULL, 0), COLLECTOR_PATH, collectors.size() + startIndex);
        mkdir(path, 0777);

        collectors.push_back(Collector::newInstance(path));
    }

    return true;
}

bool ComponentController::startNode(int count, int startIndex) {

    for (int i = 0; i < count; i++) {

        char path[PATH_MAX];
        sprintf(path, "%s/%s_%ld", getcwd(nullptr, 0), NODE_PATH, nodes.size() + startIndex);
        mkdir(path, 0777);

        nodes.push_back(Node::newInstance(path));
    }

    return true;
}

bool ComponentController::setInterfaces(int ind1, int ind2) {

    DeviceList *deviceList = DeviceList::getInstance();
    deviceList->setActive(ind1, ind2);

    return true;
}
