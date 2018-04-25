//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_COLLECTORMANAGER_H
#define BANKOR_COLLECTORMANAGER_H

#include "Common.h"

typedef std::pair<long, const char*> TypeWaitingCollector;

class CollectorManager {

    std::mutex mutex;
    std::deque<TypeWaitingCollector> waitingList;

public:

    bool add(long, const char*);
    TypeWaitingCollector get();
    size_t getCount();
    void clear();
};


#endif //BANKOR_COLLECTORMANAGER_H
