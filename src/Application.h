//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_APPLICATION_H
#define BANKOR_APPLICATION_H

#include "Common.h"
#include "DeviceList.h"
#include "Distributor.h"
#include "Collector.h"

class Application {

    bool initStart;

    Distributor *distributor = NULL;
    std::vector<Collector*> collectors;
    std::vector<Node*> nodes;

public:

    Application(int, int, bool = true);

    void setStartState(bool);
    bool getStartState();

    Distributor* getDistributor();
    Collector* getCollector(int);
    Node* getNode(int);

    bool startDistributor();
    bool startCollector(int);
    bool startNode(int);

};


#endif //BANKOR_APPLICATION_H
