//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef NODEMANAGER_H
#define	NODEMANAGER_H

#include "Node.h"
#include "Connector.h"
#include "Util.h"
#include "StopWatch.h"
#include "NodeObject.h"
#include "NodeWatchdog.h"

typedef std::map<long, NodeObject> TypeNodeList;

class NodeManager {
private:

    std::mutex mutex;
    TypeNodeList nodes;

public:

	NodeManager();

	virtual ~NodeManager();

	bool add(long);
	bool setState(long, NODE_STATES);

	long getIdle();

	void clear();

};

#endif	/* DISTRIBUTOR_H */
