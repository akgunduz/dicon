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
    size_t reqCount;
    CollectorRequest(long _collID, size_t _reqCount)
    : collID(_collID), reqCount(_reqCount){};
};

class CollectorManager : public ComponentManager {

    std::mutex collMutex;

    std::map<long, CollectorObject> collList;
    std::deque<struct CollectorRequest> collReqList;

protected:

    ComponentUnit* createUnit(ARCH, long, Address&) final;

public:

    CollectorManager(HostUnit *, bool);
    ~CollectorManager() override;

    COLLSTATES getState(long);
    void setState(long, COLLSTATES);

    bool addRequest(long, size_t);
    CollectorRequest* getRequest();
    void updateRequest(size_t);
    void removeRequest();
};

#endif //DICON_COLLECTORMANAGER_H
