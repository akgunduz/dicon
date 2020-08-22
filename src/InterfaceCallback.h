//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_INTERFACECALLBACK_H
#define DICON_INTERFACECALLBACK_H

#include "Message.h"
#include "SchedulerItem.h"

typedef bool (*TypeSchedulerCB)(void*, TypeSchedulerItem);
typedef TypeHostUnit& (*TypeGetHostCB)(void*);
typedef int (*TypeGetIDCB)(void*);

class InterfaceSchedulerCB {
public:
    TypeSchedulerCB schedulerCB;
    void *arg;
    InterfaceSchedulerCB(TypeSchedulerCB, void *);
    ~InterfaceSchedulerCB();
};

class InterfaceHostCB {
public:
    TypeGetHostCB hostCB;
    void *arg;
    InterfaceHostCB(TypeGetHostCB, void *);
    ~InterfaceHostCB();
};

#endif //DICON_INTERFACECALLBACK_H
