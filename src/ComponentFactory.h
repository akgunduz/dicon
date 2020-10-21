//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#ifndef DICON_COMPONENTFACTORY_H
#define DICON_COMPONENTFACTORY_H

#include "Distributor.h"
#include "Collector.h"
#include "Node.h"

class ComponentFactory {

    TypeComponentMapList components;

    int commInterfaces[2]{};

    static ComponentFactory *instance;

    explicit ComponentFactory(const int*);

public:

    static ComponentFactory* newInstance(int*);
    ~ComponentFactory();

    bool hasDistributor();
    TypeDistributor& getDistributor();
    size_t getCollectorCount();
    TypeCollector& getCollector(long);
    TypeComponentList& getCollectors();
    size_t getNodeCount();
    TypeNode& getNode(long);
    TypeComponentList& getNodes();

    bool startDistributor(bool);
    bool startCollector(int);
    TypeCollector& startCollector();
    bool startNode(int);
    TypeNode& startNode();
};

#endif //DICON_COMPONENTFACTORY_H
