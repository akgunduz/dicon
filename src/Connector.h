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

private:

	static std::vector<Device*>deviceList;
    static Device *selectedDevices[2] ;
	static bool devicesInitialized;

    Connector(){};

public:

	static bool createDevices();
	static std::vector<Device*>* getDevices();
	static Device* getDevice(int);
	static unsigned long getCount();

    static void setSelectedDevices(int, int);
    static Device* getSelectedDevice(unsigned char);

    static Interface* createInterface(Unit component, Device* device, const InterfaceCallback *cb);
};


#endif //__Connector_H_
