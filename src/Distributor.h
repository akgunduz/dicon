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

    bool processWaitingList(ComponentObject, Message *);

	bool send2CollectorWakeupMsg(ComponentObject, long);
	bool send2CollectorIDMsg(ComponentObject, long, int);
	bool send2CollectorNodeMsg(ComponentObject, long, const char*, long, int);

    bool send2NodeWakeupMsg(ComponentObject, long);
    bool send2NodeIDMsg(ComponentObject, long, int);


public:


	~Distributor();
    static Distributor* newInstance(const char* path);

    bool clear();

    bool sendWakeupMessage(ComponentObject);
    bool sendWakeupMessagesAll(bool);



};

#endif	/* DISTRIBUTOR_H */
