//
// Created by Haluk AKGUNDUZ on 21/03/18.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "JobInfo.h"

bool JobInfo::add(ExecutorItem *item, NodeInfo node) {

    nodes_address[node] = item;
    nodes_job[item] = node;

    return true;
}

bool JobInfo::remove(ExecutorItem *item) {

    std::map<const ExecutorItem*, NodeInfo>::iterator search = nodes_job.find(item);
    if (search == nodes_job.end()) {
        return false;
    }
    nodes_job.erase(search->first);
    nodes_address.erase(search->second);
    return true;
}

bool JobInfo::remove(NodeInfo node) {

    std::map<const NodeInfo, ExecutorItem*, cmp_node>::iterator search = nodes_address.find(node);
    if (search == nodes_address.end()) {
        return false;
    }
    nodes_address.erase(search->first);
    nodes_job.erase(search->second);
    return true;
}

ExecutorItem *JobInfo::get(NodeInfo node) {

    std::map<const NodeInfo, ExecutorItem*, cmp_node>::iterator search = nodes_address.find(node);
    if (search == nodes_address.end()) {
        return NULL;
    }

    return search->second;
}

ExecutorItem *JobInfo::get(long address) {

    NodeInfo node = NodeInfo(address, ARCH_MAX);
    std::map<const NodeInfo, ExecutorItem*, cmp_node>::iterator search = nodes_address.find(node);
    if (search == nodes_address.end()) {
        return NULL;
    }

    return search->second;
}


NodeInfo JobInfo::get(ExecutorItem *item) {

    std::map<const ExecutorItem*, NodeInfo>::iterator search = nodes_job.find(item);
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

    ExecutorItem* job = get(address);
    if (job == NULL) {
        return NodeInfo();
    }

    return get(job);
}
