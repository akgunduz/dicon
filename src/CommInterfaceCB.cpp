//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "CommInterfaceCB.h"
#include "Log.h"

CommInterfaceCB::CommInterfaceCB(TypeSchedulerCB _schedulerCB, void *_arg)
    : schedulerCB(_schedulerCB), arg(_arg) {

}

CommInterfaceCB::~CommInterfaceCB() {

    LOGP_T("Deallocating CommInterfaceCB");
}
