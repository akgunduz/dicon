//
// Created by Haluk AKGUNDUZ on 21/03/18.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "JobInfo.h"

bool JobInfo::add(ExecutorItem *item, long address) {

    nodes_address[address] = item;
    nodes_job[item] = address;

    return true;
}

bool JobInfo::remove(ExecutorItem *item) {

    TypeExecAddressList::iterator search = nodes_job.find(item);
    if (search == nodes_job.end()) {
        return false;
    }
    nodes_job.erase(search->first);
    nodes_address.erase(search->second);
    return true;
}

bool JobInfo::remove(long address) {

    TypeAddressExecList::iterator search = nodes_address.find(address);
    if (search == nodes_address.end()) {
        return false;
    }
    nodes_address.erase(search->first);
    nodes_job.erase(search->second);
    return true;
}

ExecutorItem *JobInfo::get(long address) {

    TypeAddressExecList::iterator search = nodes_address.find(address);
    if (search == nodes_address.end()) {
        return NULL;
    }

    return search->second;
}

long JobInfo::get(ExecutorItem *item) {

    TypeExecAddressList::iterator search = nodes_job.find(item);
    if (search == nodes_job.end()) {
        return 0;
    }

    return search->second;
}

void JobInfo::clear() {

    nodes_job.clear();
    nodes_address.clear();
}

long JobInfo::getNode(long address) {

    ExecutorItem* job = get(address);
    if (job == NULL) {
        return 0;
    }

    return get(job);
}
