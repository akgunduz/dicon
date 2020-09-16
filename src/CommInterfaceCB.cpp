//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "Log.h"
#include "CommInterfaceCB.h"

CommInterfaceCB::CommInterfaceCB(TypeSchedulerCB _schedulerCB, void *_arg)
    : schedulerCB(_schedulerCB), arg(_arg) {

}

CommInterfaceCB::~CommInterfaceCB() {

    LOGP_T("Deallocating InterfaceSchedulerCB");
}
