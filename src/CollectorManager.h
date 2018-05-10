//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_COLLECTORMANAGER_H
#define BANKOR_COLLECTORMANAGER_H

#include "ComponentManager.h"

//typedef std::pair<long, std::string> TypeWaitingCollector;
typedef long TypeWaitingCollector;

class CollectorManager : public ComponentManager {

    std::deque<TypeWaitingCollector> waitingList;

public:

    CollectorManager();
    ~CollectorManager();
    bool addWaiting(long);
    TypeWaitingCollector getWaiting();
    size_t getWaitingCount();
    void clearWaiting();

    void setObject(int, long);
};


#endif //BANKOR_COLLECTORMANAGER_H
