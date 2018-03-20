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

	Interface* _interface;

	static std::vector<Device>deviceList;
    static Device *selectedDevices[2] ;

public:
	Connector(Unit, Device*, const InterfaceCallback *);
	bool send(long, Message*);
	bool send(Message*);
	bool put(long, Message*);
	long getAddress();
	INTERFACES getInterfaceType();
	std::vector<long> getAddressList();
	virtual ~Connector();

	static bool createDevices();
	static std::vector<Device>* getDevices();
	static Device* getDevice(unsigned long);
	static unsigned long getCount();

    static void setSelectedDevices(unsigned char, unsigned char);
    static Device* getSelectedDevice(unsigned char);
};


#endif //__Connector_H_
