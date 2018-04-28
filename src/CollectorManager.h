//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_COLLECTORMANAGER_H
#define BANKOR_COLLECTORMANAGER_H

#include "Common.h"

typedef std::pair<long, std::string> TypeWaitingCollector;
typedef std::map<long, int> TypeCollectorList;

class CollectorManager {

    std::mutex mutex;
    TypeCollectorList collectors;
    std::deque<TypeWaitingCollector> waitingList;

    static int idCounter;

public:

    virtual ~CollectorManager();
    bool addWaiting(long, std::string);
    TypeWaitingCollector getWaiting();
    size_t getWaitingCount();
    void clearWaiting();
    int add(long);
    int getID(long);

    static int getFreeID();

    void clear();
};


#endif //BANKOR_COLLECTORMANAGER_H
