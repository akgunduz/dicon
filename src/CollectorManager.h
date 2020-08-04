//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTORMANAGER_H
#define DICON_COLLECTORMANAGER_H

#include "ComponentManager.h"
#include "NodeHost.h"
#include "CollectorObject.h"

struct CollectorRequest {
    long collID;
    int reqCount;
    CollectorRequest(long _collID, int _reqCount)
    : collID(_collID), reqCount(_reqCount){};
};

class CollectorManager : public ComponentManager {

    std::mutex collMutex;

    std::map<long, CollectorObject> collList;
    std::deque<struct CollectorRequest> collReqList;

protected:

    ComponentUnit* createUnit(long, Address&) final;

public:

    CollectorManager();
    ~CollectorManager() override;

    COLLSTATES getState(long);
    void setState(long, COLLSTATES);

    bool addRequest(long, int);
    CollectorRequest* getRequest();
    void updateRequest(int);
    void removeRequest();
};

#endif //DICON_COLLECTORMANAGER_H
