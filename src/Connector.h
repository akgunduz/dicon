//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_CONNECTOR_H
#define BANKOR_CONNECTOR_H

#include "Log.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"
#include "AddressHelper.h"

class Connector {

private:

    Connector(){};

public:

    static Interface* createInterface(Device*,
                                      const InterfaceSchedulerCB*,
                                      const InterfaceHostCB*);
};


#endif //BANKOR_CONNECTOR_H
