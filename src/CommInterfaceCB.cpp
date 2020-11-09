//
// Created by Haluk AKGUNDUZ on 18.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#include "CommInterfaceCB.h"
#include "Log.h"

CommInterfaceCB::CommInterfaceCB(TypeSchedulerCB _schedulerCB, void *_arg)
    : schedulerCB(_schedulerCB), arg(_arg) {

}

CommInterfaceCB::~CommInterfaceCB() {

    LOGP_T("Deallocating CommInterfaceCB");
}
