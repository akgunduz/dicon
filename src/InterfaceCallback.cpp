//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "InterfaceCallback.h"

InterfaceSchedulerCB::InterfaceSchedulerCB(TypeSchedulerCB _schedulerCB, void *_arg)
    : schedulerCB(_schedulerCB), arg(_arg) {

}

InterfaceSchedulerCB::~InterfaceSchedulerCB() {

    LOGP_T("Deallocating InterfaceSchedulerCB");
}

InterfaceHostCB::InterfaceHostCB(TypeGetHostCB _hostCB, void *_arg)
    : hostCB(_hostCB), arg(_arg) {

}

InterfaceHostCB::~InterfaceHostCB() {

    LOGP_T("Deallocating InterfaceHostCB");
}
