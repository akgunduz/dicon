//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#include "ProcessInfo.h"

ProcessInfo::ProcessInfo(long id, ProcessItem *item) :
    id(id), item(item), state(PROCESS_STATE_NOTSTARTED) {

}

ProcessItem *ProcessInfo::get() {

    return item;
}

long ProcessInfo::getID() {

    return id;
}

PROCESS_STATE ProcessInfo::getState() {

    return state;
}

void ProcessInfo::setState(PROCESS_STATE state) {

    this->state = state;
}

