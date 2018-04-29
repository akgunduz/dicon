//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#include "ExecutorInfo.h"

ExecutorInfo::ExecutorInfo(long id, ExecutorItem *item) :
    id(id), item(item), state(PROCESS_STATE_NOTSTARTED) {

}

ExecutorItem *ExecutorInfo::get() {

    return item;
}

long ExecutorInfo::getID() {

    return id;
}

PROCESS_STATE ExecutorInfo::getState() {

    return state;
}

void ExecutorInfo::setState(PROCESS_STATE state) {

    this->state = state;
}

