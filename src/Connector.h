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
#include "AddressHelper.h"

class Connector {

private:

    Connector(){};

public:

    static Interface* createInterface(COMPONENT component, Device* device, const InterfaceCallback *cb);
};


#endif //__Connector_H_
