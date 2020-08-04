//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_INTERFACECALLBACK_H
#define DICON_INTERFACECALLBACK_H

#include "Message.h"
#include "SchedulerItem.h"

typedef bool (*TypeSchedulerCB)(void*, SchedulerItem*);
typedef HostUnit& (*TypeGetHostCB)(void*);
typedef int (*TypeGetIDCB)(void*);

class InterfaceSchedulerCB {
public:
    TypeSchedulerCB schedulerCB;
    void *arg;
    InterfaceSchedulerCB(TypeSchedulerCB _schedulerCB, void *_arg) :
            schedulerCB(_schedulerCB), arg(_arg) {}
};

class InterfaceHostCB {
public:
    TypeGetHostCB hostCB;
    void *arg;
    InterfaceHostCB(TypeGetHostCB _hostCB, void *_arg) :
            hostCB(_hostCB), arg(_arg) {}
};

#endif //DICON_INTERFACECALLBACK_H
