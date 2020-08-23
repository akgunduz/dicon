//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTORMANAGER_H
#define DICON_COLLECTORMANAGER_H

#include "ComponentManager.h"
#include "NodeHost.h"
#include "CollectorObject.h"

typedef struct CollectorRequest {

    long collID;

    size_t reqCount;

    CollectorRequest(long _collID, size_t _reqCount)
        : collID(_collID), reqCount(_reqCount){

    };

} TypeCollectorRequest;

class CollectorManager : public ComponentManager {

    std::mutex collMutex;

    std::map<long, CollectorObject> collList;
    std::deque<TypeCollectorRequest> collReqList;

protected:

    TypeComponentUnit createUnit(ARCH, long, Address&) final;

public:

    CollectorManager(const TypeHostUnit&, bool);
    ~CollectorManager() override;

    COLLSTATES getState(long);
    void setState(long, COLLSTATES);

    bool addRequest(long, size_t);
    CollectorRequest* getRequest();
    void updateRequest(size_t);
    void removeRequest();
};

#endif //DICON_COLLECTORMANAGER_H
