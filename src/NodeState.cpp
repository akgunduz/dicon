//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "NodeState.h"

const char* sNodeStates[] = {
        "IDLE",
        "PREBUSY",
        "BUSY",
        "DEAD",
};

const char* NodeState::getName(NODESTATES state) {

    return sNodeStates[state];
}
