//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef NODEMANAGER_H
#define	NODEMANAGER_H

#include "NodeObject.h"
#include "ComponentManager.h"

class NodeManager : public ComponentManager {

private:

    std::mutex nodeMutex;

public:

	NodeManager();
	~NodeManager() override;

    NODESTATES getState(long);
	void setState(long, NODESTATES);

	long getAddress(long);
    NodeObject getIdle();
	int getIdleCount();

    ComponentInfo getAssigned(long);
	void setAssigned(long, ComponentInfo &);
	void setAssigned(long, long, long);

    ComponentObject* createObject(long, long) final;
};

#endif	/* DISTRIBUTOR_H */
