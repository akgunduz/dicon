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

class Distributor : public Component {
private:

	NodeManager *nodeManager;

	std::deque<long> collectorWaitingList;

    virtual bool processDistributorMsg(long, Message *);
    virtual bool processCollectorMsg(long, Message *);
    virtual bool processNodeMsg(long, Message *);

    bool send2NodeMsg(long, MSG_TYPE);
    bool send2CollectorMsg(long, MSG_TYPE);

public:

	Distributor(const char *, double);
	virtual ~Distributor();

    bool reset();
    static bool onTimeOut(Component*, NodeObject*);
    static bool onWakeup(Component*);
    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll();



};

#endif	/* DISTRIBUTOR_H */
