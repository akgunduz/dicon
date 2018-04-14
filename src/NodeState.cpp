//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include <clocale>
#include "NodeState.h"

const char* sNodeStates[] = {
        "IDLE",
        "BUSY",
        "PREBUSY"
};

const char* NodeState::getName(NODE_STATES state) {

    return sNodeStates[state];
}
