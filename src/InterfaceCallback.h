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
    TypeSchedulerCB cb;
    void *arg;
    InterfaceSchedulerCB(TypeSchedulerCB cb, void *arg) :
            cb(cb), arg(arg) {}
};

class InterfaceHostCB {
public:
    TypeGetHostCB hcb;
    void *arg;
    InterfaceHostCB(TypeGetHostCB hcb, void *arg) :
            hcb(hcb), arg(arg) {}
};


#endif //DICON_INTERFACECALLBACK_H
