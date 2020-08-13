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

	NodeManager(HostUnit *);
	~NodeManager() override;

    NODESTATES getState(long);
	void setState(long, NODESTATES);

    Address& getAddress(long);
    NodeUnit getIdle();
	int getIdleCount();

    ComponentUnit getAssigned(long);
	void setAssigned(long, ComponentUnit &);
	void setAssigned(long, long, Address&);
	void releaseAssigned(long);

    ComponentUnit* createUnit(long, Address&) final;
};

#endif //DICON_NODEMANAGER_H