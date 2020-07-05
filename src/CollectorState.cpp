//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollState.h"

const char* sCollStates[] = {
        "START",
        "IDLE",
        "BUSY",
};

const char* CollState::getName(COLLSTATES state) {

    return sCollStates[state];
}
