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
	~NodeManager();

    NODESTATES getState(int);
	void setState(int, NODESTATES);

	long getAddress(int);
    NodeObject getIdle();
	int getIdleCount();

    ComponentInfo getAssigned(int);
	void setAssigned(int, ComponentInfo &);
	void setAssigned(int, int, long);

    ComponentObject* createObject(int, long) final;
};

#endif	/* DISTRIBUTOR_H */
