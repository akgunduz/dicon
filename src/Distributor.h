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

    bool processCollectorAliveMsg(long, Message *);
    bool processCollectorNodeMsg(long, Message *);

    bool processNodeReadyMsg(long, Message *);
    bool processNodeAliveMsg(long, Message *);
    bool processNodeBusyMsg(long, Message *);
    bool processNodeIDMsg(long, Message *);

	bool send2CollectorWakeupMsg(long);
	bool send2CollectorIDMsg(long, int);
	bool send2CollectorNodeMsg(long, const char*, long);

    bool send2NodeWakeupMsg(long);
    bool send2NodeIDMsg(long, int);


public:


	~Distributor();
    static Distributor* newInstance();

    bool reset();

    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll();



};

#endif	/* DISTRIBUTOR_H */
