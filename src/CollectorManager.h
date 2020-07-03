//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTORMANAGER_H
#define DICON_COLLECTORMANAGER_H

#include "ComponentManager.h"
#include "NodeObject.h"

typedef std::pair<long, std::string> TypeWaitingCollector;

class CollectorManager : public ComponentManager {

    std::deque<TypeWaitingCollector> waitingList;

public:

    CollectorManager();
    ~CollectorManager();
    bool addWaiting(long, std::string);
    TypeWaitingCollector getWaiting();
    size_t getWaitingCount();
    void clearWaiting();

    void setObject(int, long);

    //ComponentObject* getWaiting();
    bool attachNode(long, const NodeObject&);
    bool detachNode(long);
};


#endif //DICON_COLLECTORMANAGER_H
