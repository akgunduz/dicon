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
#include "NodeItem.h"
#include "NodeWatchdog.h"

struct NodeManagerArgument {
    Connector *nodeConnector;
    NodeItem *nodeMap;
    long collectorAddress;
    fTimeoutCB timeoutCB;
    NodeManagerArgument(Connector *cc, fTimeoutCB cb, NodeItem *c, long a) :
            nodeConnector(cc), timeoutCB(cb), nodeMap(c), collectorAddress(a) {}
};

class NodeManager {
private:

	std::map<long, NodeItem *> nodes;

    Component *component;
    fTimeoutCB timeoutCB;
    fWakeupCB wakeupCB;

    double backupRate = 0;
	int readyBackup = 0;
	int totalBackup = 0;

    NodeWatchdog *nodeWatchdog = nullptr;

public:

	NodeManager(Component*, fTimeoutCB, fWakeupCB, double);

	virtual ~NodeManager();

	bool resetTimes();
	bool setIdle(long, short, double);
	bool setBusy(long);
	bool remove(long);
	bool validate(long, short);
	bool add(long, short);

	NodeItem* getIdle(long);

	void clear();

};

#endif	/* DISTRIBUTOR_H */
