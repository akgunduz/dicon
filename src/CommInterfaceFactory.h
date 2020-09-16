//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMINTERFACEFACTORY_H
#define DICON_COMMINTERFACEFACTORY_H

#include "Log.h"
#include "Scheduler.h"
#include "CommInterface.h"

class CommInterfaceFactory {

    CommInterfaceFactory()= default;

public:

    static TypeInterface createInterface(TypeHostUnit, const TypeDevice&,
                                         const CommInterfaceCB *);
};

#endif //DICON_COMMINTERFACEFACTORY_H
