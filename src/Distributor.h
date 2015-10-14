//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DISTRIBUTOR_H
#define	DISTRIBUTOR_H

#include "Component.h"
#include "Client.h"
#include "Connector.h"
#include "Tools.h"
#include "ClientManager.h"

class Distributor : public Component {
private:

	ClientManager *clientManager;

	std::deque<long> collectorWaitingList;

	DiffTime collStartTime;

public:

	Distributor(int, int, const char *, double);

	virtual ~Distributor();

	static bool timeoutCallback(Connector *, long, long);

	bool reset();

	bool send2ClientMsg(long, uint8_t);
	bool send2CollectorMsg(long, uint8_t);
	static bool sendWakeupMessage(Connector *);
	bool sendWakeupMessagesAll();

	virtual bool processDistributorMsg(long, Message *);
	virtual bool processCollectorMsg(long, Message *);
	virtual bool processClientMsg(long, Message *);
};

#endif	/* DISTRIBUTOR_H */
