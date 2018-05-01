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

public:

	NodeManager();

	~NodeManager();

	bool setState(long, NODE_STATES);

	long getIdle();

    void setObject(int, long);
};

#endif	/* DISTRIBUTOR_H */
