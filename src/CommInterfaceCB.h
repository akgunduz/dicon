//
// Created by Haluk Akgunduz on 10/18/15.
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
