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

struct NodeManagerArgument {
    Connector *nodeConnector;
    NodeObject *nodeMap;
    long collectorAddress;
    fTimeoutCB timeoutCB;
    NodeManagerArgument(Connector *cc, fTimeoutCB cb, NodeObject *c, long a) :
            nodeConnector(cc), timeoutCB(cb), nodeMap(c), collectorAddress(a) {}
};

typedef std::map<NodeInfo, NodeObject *, cmp_node> TypeNodeList;

class NodeManager {
private:

    TypeNodeList nodes;

    Component *component;
    fTimeoutCB timeoutCB;

#ifndef DISABLE_BACKUP
    double backupRate;
	int readyBackup;
	int totalBackup;
#endif

#ifndef DISABLE_RECOVERY
    fWakeupCB wakeupCB;
    NodeWatchdog *nodeWatchdog;
#endif

public:

	NodeManager(Component*, fTimeoutCB, fWakeupCB, double);

	virtual ~NodeManager();

	bool setIdle(NodeInfo);
	bool setBusy(NodeInfo);
	bool remove(NodeInfo);
	bool validate(NodeInfo);
	bool add(NodeInfo);

	NodeObject* getIdle();

	void clear();

};

#endif	/* DISTRIBUTOR_H */
