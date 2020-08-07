//
// Created by akgun on 9.07.2020.
//

#include "ProcessState.h"

const char* sProcessStates[] = {
        "INVALID",
        "DEPENDENT",
        "READY",
        "STARTED",
        "ENDED",
};

const char* ProcessState::getName(PROCESS_STATE state) {

    return sProcessStates[state];
}
