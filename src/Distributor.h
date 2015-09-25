//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DISTRIBUTOR_H
#define	DISTRIBUTOR_H

#include <atomic>
#include "Client.h"
#include "Connector.h"
#include "Tools.h"
#include "ClientManager.h"

class Distributor {
private:

	Connector *mCollectorConnector;
	Connector *mClientConnector;

	std::string mRootPath;

	CallBack *mCallback;

	ClientManager *mClientManager;

	std::deque<uint64_t> mCollectorWaitingList;

	DiffTime mCollStartTime;

	static bool receiveCB(void *, uint64_t, Message *);

public:

	Distributor(uint32_t, uint32_t, const std::string &, double);

	virtual ~Distributor();
	uint64_t getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	static bool timeoutCallback(Connector *, uint64_t, uint64_t);

	bool reset();

	bool send2ClientMsg(uint64_t, uint8_t);
	bool send2CollectorMsg(uint64_t, uint8_t);
	static bool sendWakeupMessage(Connector *);
	bool sendWakeupMessagesAll();

	bool processCollectorMsg(uint64_t, Message *);
	bool processClientMsg(uint64_t, Message *);

	std::string getRootPath();
};

#endif	/* DISTRIBUTOR_H */
