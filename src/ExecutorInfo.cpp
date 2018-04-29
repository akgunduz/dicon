//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#include "ExecutorInfo.h"

ExecutorInfo::ExecutorInfo(long id, ExecutorItem *item) :
    id(id), item(item), is_processed(false) {

}

ExecutorItem *ExecutorInfo::get() {

    return item;
}

long ExecutorInfo::getID() {

    return id;
}

bool ExecutorInfo::isProcessed() {

    return is_processed;
}

void ExecutorInfo::setProcessState(bool is_processed) {

    this->is_processed = is_processed;
}

