//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#ifndef BANKOR_COMPONENTCONTROLLER_H
#define BANKOR_COMPONENTCONTROLLER_H


#include "Distributor.h"
#include "Collector.h"

class ComponentController {

    Distributor *distributor = NULL;
    std::vector<Collector*> collectors;
    std::vector<Node*> nodes;

    static ComponentController *instance;

    ComponentController(int, int);

public:

    static ComponentController* newInstance(int, int);
    ~ComponentController();

    long getDistributorCount();
    Distributor* getDistributor();

    long getCollectorCount();
    Collector* getCollector(int);

    long getNodeCount();
    Node* getNode(int);

    bool startDistributor();
    bool startCollector(int, int = 1);
    bool startNode(int, int = 1);

    bool setInterfaces(int, int);
};


#endif //BANKOR_COMPONENTCONTROLLER_H
