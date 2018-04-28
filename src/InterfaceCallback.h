//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_INTERFACECALLBACK_H
#define BANKOR_INTERFACECALLBACK_H

#include "Message.h"
#include "SchedulerItem.h"

typedef bool (*TypeSchedulerCB)(void*, SchedulerItem*);
typedef int (*TypeGetHostCB)(void*);
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
    TypeGetIDCB icb;
    void *arg;
    InterfaceHostCB(TypeGetHostCB hcb, TypeGetIDCB icb, void *arg) :
            hcb(hcb), icb(icb), arg(arg) {}
};


#endif //BANKOR_INTERFACECALLBACK_H
