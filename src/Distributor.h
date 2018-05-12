//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DISTRIBUTOR_H
#define	DISTRIBUTOR_H

#include "Component.h"
#include "Node.h"
#include "Connector.h"
#include "Util.h"
#include "NodeManager.h"
#include "CollectorManager.h"

class Distributor : public Component {
private:

    static Distributor *instance;

	NodeManager *nodeManager;

	CollectorManager *collectorManager;

    Distributor(const char *);

    bool processCollectorAliveMsg(ComponentObject, Message *);
    bool processCollectorNodeMsg(ComponentObject, Message *);

    bool processNodeReadyMsg(ComponentObject, Message *);
    bool processNodeAliveMsg(ComponentObject, Message *);
    bool processNodeBusyMsg(ComponentObject, Message *);
    bool processNodeIDMsg(ComponentObject, Message *);

    bool processWaitingList(long, Uuid, long);

	bool send2CollectorWakeupMsg(ComponentObject);
	bool send2CollectorIDMsg(ComponentObject, int);
	bool send2CollectorNodeMsg(ComponentObject, long, int, Uuid);
	bool send2CollectorReadyMsg(ComponentObject, Uuid);

    bool send2NodeWakeupMsg(ComponentObject);
    bool send2NodeIDMsg(ComponentObject, int);
	bool send2NodeProcessMsg(ComponentObject, Uuid, int, long);


public:


	~Distributor();
    static Distributor* newInstance(const char* path);

    bool clear();

    bool sendWakeupMessage(ComponentObject);
    bool sendWakeupMessagesAll(bool);



};

#endif	/* DISTRIBUTOR_H */
