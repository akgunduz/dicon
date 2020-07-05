//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTORMANAGER_H
#define DICON_COLLECTORMANAGER_H

#include "ComponentManager.h"
#include "NodeObject.h"
#include "CollectorObject.h"

struct CollectorRequest {
    int collID;
    int reqCount;
    CollectorRequest(int _collID, int _reqCount)
    : collID(_collID), reqCount(_reqCount){};
};

class CollectorManager : public ComponentManager {

    std::mutex collMutex;

    std::map<int, CollectorObject> collList;
    std::deque<struct CollectorRequest> collReqList;

protected:

    ComponentObject* createObject(int, long) final;

public:

    CollectorManager();
    ~CollectorManager();

    COLLSTATES getState(int);
    void setState(int, COLLSTATES);

    bool addRequest(int, int);
    CollectorRequest* getRequest();
    void updateRequest(int);
    void removeRequest();
};


#endif //DICON_COLLECTORMANAGER_H
