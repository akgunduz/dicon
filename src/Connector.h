//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONNECTOR_H
#define DICON_CONNECTOR_H

#include "Log.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"
#include "Address.h"

class Connector {

    Connector()= default;

public:

    static TypeInterface createInterface(TypeDevice&,
                                      const InterfaceSchedulerCB*,
                                      const InterfaceHostCB*);
};

#endif //DICON_CONNECTOR_H
