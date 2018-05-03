//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#include "ComponentController.h"

ComponentController *ComponentController::instance = NULL;

ComponentController *ComponentController::newInstance(int ind1, int ind2) {

    if (instance) {
        return instance;
    }

    instance = new ComponentController(ind1, ind2);
    return instance;
}

ComponentController::ComponentController(int ind1, int ind2) {

    setInterfaces(ind1, ind2);
}

Distributor *ComponentController::getDistributor() {

    return distributor;
}

Collector *ComponentController::getCollector(int index) {

    return collectors[index];
}

Node *ComponentController::getNode(int index) {

    return nodes[index];
}

bool ComponentController::startDistributor() {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(NULL, 0), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    distributor = Distributor::newInstance(path);

    return true;
}

bool ComponentController::startCollector(int count) {

    for (int i = 0; i < count; i++) {

        char path[PATH_MAX];
        sprintf(path, "%s/%s_%ld", getcwd(NULL, 0), COLLECTOR_PATH, collectors.size() + 1);
        mkdir(path, 0777);

        collectors.push_back(Collector::newInstance(path));
    }

    return true;
}

bool ComponentController::startNode(int count) {

    for (int i = 0; i < count; i++) {

        char path[PATH_MAX];
        sprintf(path, "%s/%s_%ld", getcwd(NULL, 0), NODE_PATH, nodes.size() + 1);
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

