//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DISTRIBUTOR_H
#define	DISTRIBUTOR_H

#include "Client.h"
#include "Connector.h"
#include "Tools.h"
#include "ClientManager.h"

class Distributor {
private:

	Connector *collectorConnector;
	Connector *clientConnector;

	std::string rootPath;

	InterfaceCallback *callback;

	ClientManager *clientManager;

	std::deque<long> collectorWaitingList;

	DiffTime collStartTime;

	static bool receiveCB(void *, long, Message *);

public:

	Distributor(uint32_t, uint32_t, const std::string &, double);

	virtual ~Distributor();
	long getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	static bool timeoutCallback(Connector *, long, long);

	bool reset();

	bool send2ClientMsg(long, uint8_t);
	bool send2CollectorMsg(long, uint8_t);
	static bool sendWakeupMessage(Connector *);
	bool sendWakeupMessagesAll();

	bool processCollectorMsg(long, Message *);
	bool processClientMsg(long, Message *);

	std::string getRootPath();
};

#endif	/* DISTRIBUTOR_H */
