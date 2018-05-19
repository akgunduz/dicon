//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_NODEMANAGER_H
#define	BANKOR_NODEMANAGER_H

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

#endif	/* BANKOR_NODEMANAGER_H */
