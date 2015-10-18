//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Connector_H_
#define __Connector_H_

#include "Log.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"
#include "Address.h"

class Connector {
	bool initialized = false;
	Interface* _interface;
public:
	Connector(Unit, int, const InterfaceCallback *, const char *);
	bool send(long, Message*);
	bool put(long, Message*);
	long getAddress();
	INTERFACES getInterfaceType();
	Interface* getInterface();
	std::vector<long> getAddressList();
	const char * getRootPath();
	virtual ~Connector();
};


#endif //__Connector_H_
