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

    bool processCollectorAliveMsg(ComponentObject, long, Message *);
    bool processCollectorNodeMsg(ComponentObject, long, Message *);

    bool processNodeReadyMsg(ComponentObject, long, Message *);
    bool processNodeAliveMsg(ComponentObject, long, Message *);
    bool processNodeBusyMsg(ComponentObject, long, Message *);
    bool processNodeIDMsg(ComponentObject, long, Message *);

	bool send2CollectorWakeupMsg(long);
	bool send2CollectorIDMsg(long, int);
	bool send2CollectorNodeMsg(long, const char*, long);

    bool send2NodeWakeupMsg(long);
    bool send2NodeIDMsg(long, int);


public:


	~Distributor();
    static Distributor* newInstance(const char* path);

    bool reset();

    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll();



};

#endif	/* DISTRIBUTOR_H */
