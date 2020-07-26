//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#ifndef DICON_COMPONENTCONTROLLER_H
#define DICON_COMPONENTCONTROLLER_H


#include "Distributor.h"
#include "Collector.h"
#include "Node.h"

class ComponentController {

    Distributor *distributor = nullptr;
    std::vector<Collector*> collectors;
    std::vector<Node*> nodes;

    int interface[2]{};

    static ComponentController *instance;

    ComponentController(int*);

public:

    static ComponentController* newInstance(int*);
    ~ComponentController();

    Distributor* getDistributor();
    long getCollectorCount();
    Collector* getCollector(long);
    long getNodeCount();
    Node* getNode(long);

    bool startDistributor();
    bool startCollector(int);
    bool startNode(int);
};


#endif //DICON_COMPONENTCONTROLLER_H
