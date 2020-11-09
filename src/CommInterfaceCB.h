//
// Created by Haluk AKGUNDUZ on 18.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMINTERFACECB_H
#define DICON_COMMINTERFACECB_H

#include "SchedulerItem.h"

class CommInterfaceCB {
public:
    TypeSchedulerCB schedulerCB;
    void *arg;
    CommInterfaceCB(TypeSchedulerCB, void *);
    ~CommInterfaceCB();
};

#endif //DICON_COMMINTERFACECB_H
