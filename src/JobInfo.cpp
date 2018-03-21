//
// Created by Haluk AKGUNDUZ on 21/03/18.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "JobInfo.h"

bool JobInfo::add(Job *job, NodeInfo node) {

    nodes_address[node] = job;
    nodes_job[job] = node;

    return true;
}

bool JobInfo::remove(Job *job) {

    auto search = nodes_job.find(job);
    if (search == nodes_job.end()) {
        return false;
    }
    nodes_job.erase(search->first);
    nodes_address.erase(search->second);
    return true;
}

bool JobInfo::remove(NodeInfo node) {

    auto search = nodes_address.find(node);
    if (search == nodes_address.end()) {
        return false;
    }
    nodes_address.erase(search->first);
    nodes_job.erase(search->second);
    return true;
}

Job *JobInfo::get(NodeInfo node) {

    auto search = nodes_address.find(node);
    if (search == nodes_address.end()) {
        return NULL;
    }

    return search->second;
}

Job *JobInfo::get(long address) {

    NodeInfo node = NodeInfo(address, ARCH_MAX);
    auto search = nodes_address.find(node);
    if (search == nodes_address.end()) {
        return NULL;
    }

    return search->second;
}


NodeInfo JobInfo::get(Job *job) {

    auto search = nodes_job.find(job);
    if (search == nodes_job.end()) {
        return NodeInfo();
    }

    return search->second;
}

void JobInfo::clear() {

    nodes_job.clear();
    nodes_address.clear();
}

NodeInfo JobInfo::getNode(long address) {

    Job* job = get(address);
    if (job == NULL) {
        return NodeInfo();
    }

    return get(job);
}
