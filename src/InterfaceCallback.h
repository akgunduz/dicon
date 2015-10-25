//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_INTERFACECALLBACK_H
#define BANKOR_INTERFACECALLBACK_H

#include "Message.h"
#include "SchedulerItem.h"

typedef bool (*fSchedulerCB)(void*, SchedulerItem*);

class InterfaceCallback {
public:
    fSchedulerCB cb;
    void *arg;
    InterfaceCallback(fSchedulerCB _cb, void *_arg) : cb(_cb), arg(_arg) {}
};


#endif //BANKOR_INTERFACECALLBACK_H
