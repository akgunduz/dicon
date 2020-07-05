//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#include "ProcessInfo.h"

ProcessInfo ProcessInfo::invalid = {0, NULL};

ProcessInfo::ProcessInfo(int id, ProcessItem *item) :
    id(id), item(item), state(PROCESS_STATE_DEPENDENT), assigned(0) {
}

ProcessItem *ProcessInfo::get() const {

    return item;
}

int ProcessInfo::getID() const {

    return id;
}

PROCESS_STATE ProcessInfo::getState() const {

    return state;
}

void ProcessInfo::setState(PROCESS_STATE _state) {

    state = _state;
}

int ProcessInfo::getAssignedNode() {

    return assigned;
}

void ProcessInfo::setAssignedNode(int _assigned) {

    assigned = _assigned;
}

