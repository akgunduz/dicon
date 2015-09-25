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

class Connector {
	bool mInitialized = false;
	Interface* mInterface;
public:
	Connector(uint32_t, const CallBack *, const std::string &);
	bool send(uint64_t, Message*);
	uint64_t getAddress();
	INTERFACES getInterfaceType();
	Interface* getInterface();
	int getNotifier(NOTIFIER_TYPE);
	void setRootPath(std::string&);
	std::vector<uint64_t> getAddressList();
	std::string getRootPath();
	virtual ~Connector();
};


#endif //__Connector_H_
