//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_INTERFACECALLBACK_H
#define DICON_INTERFACECALLBACK_H

#include "Message.h"
#include "SchedulerItem.h"

typedef bool (*TypeSchedulerCB)(void*, const TypeSchedulerItem&);

class InterfaceSchedulerCB {
public:
    TypeSchedulerCB schedulerCB;
    void *arg;
    InterfaceSchedulerCB(TypeSchedulerCB, void *);
    ~InterfaceSchedulerCB();
};

#endif //DICON_INTERFACECALLBACK_H
