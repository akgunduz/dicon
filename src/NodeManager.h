//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODEMANAGER_H
#define	DICON_NODEMANAGER_H

#include "NodeUnit.h"
#include "ComponentManager.h"

class NodeManager : public ComponentManager {

    std::mutex nodeMutex;

public:

	NodeManager(HostUnit *, bool);
	~NodeManager() override;

    NODESTATES getState(long);
	void setState(long, NODESTATES);

    Address& getAddress(long);
    NodeUnit* getIdle(ComponentUnit*);
    size_t getIdleCount();
    NodeUnit* getBusyDead();
    size_t getBusyDeadCount();

    ComponentUnit getAssigned(long);
	void setAssigned(long, ComponentUnit &);
	void setAssigned(long, long, Address&);
	void releaseAssigned(long);

    ComponentUnit* createUnit(ARCH, long, Address&) final;
};

#endif //DICON_NODEMANAGER_H