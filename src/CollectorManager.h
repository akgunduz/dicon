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

    TypeID collID;

    size_t reqCount;

    CollectorRequest(TypeID _collID, uint32_t _reqCount)
        : collID(_collID), reqCount(_reqCount){

    };

} TypeCollectorRequest;

class CollectorManager : public ComponentManager {

    std::mutex collMutex;

    std::map<TypeID, CollectorObject> collList;
    std::deque<TypeCollectorRequest> collReqList;

protected:

    TypeComponentUnit createUnit(ARCH, TypeID, TypeAddress&) final;

public:

    CollectorManager(const TypeHostUnit&, bool);
    ~CollectorManager() override;

    COLLSTATES getState(TypeID);
    void setState(TypeID, COLLSTATES);

    bool addRequest(TypeID, uint32_t);
    CollectorRequest* getRequest();
    void updateRequest(size_t);
    void removeRequest();
};

#endif //DICON_COLLECTORMANAGER_H
