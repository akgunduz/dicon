//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#include "ExecutorInfo.h"

ExecutorInfo ExecutorInfo::invalid = {0, NULL};

ExecutorInfo::ExecutorInfo(int id, ExecutorItem *item) :
    id(id), item(item), state(PROCESS_STATE_NOTSTARTED), assigned(0) {
}

ExecutorItem *ExecutorInfo::get() {

    return item;
}

int ExecutorInfo::getID() {

    return id;
}

PROCESS_STATE ExecutorInfo::getState() {

    return state;
}

void ExecutorInfo::setState(PROCESS_STATE _state) {

    state = _state;
}

int ExecutorInfo::getAssignedNode() {

    return assigned;
}

void ExecutorInfo::setAssignedNode(int _assigned) {

    assigned = _assigned;
}

