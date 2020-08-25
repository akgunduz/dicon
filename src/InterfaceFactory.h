//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_INTERFACEFACTORY_H
#define DICON_INTERFACEFACTORY_H

#include "Log.h"
#include "Scheduler.h"
#include "CommInterface.h"
#include "Message.h"
#include "Address.h"

class InterfaceFactory {

    InterfaceFactory()= default;

public:

    static TypeInterface createInterface(const TypeHostUnit&, const TypeDevice&,
                                         const InterfaceSchedulerCB *);
};

#endif //DICON_INTERFACEFACTORY_H
