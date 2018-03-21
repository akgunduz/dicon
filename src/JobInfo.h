//
// Created by Haluk AKGUNDUZ on 21/03/18.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_JOBINFO_H
#define BANKOR_JOBINFO_H


#include "Job.h"

struct cmp_node {
    bool operator()(const NodeInfo &a, const NodeInfo &b) const {
        return a.getAddress() < b.getAddress();
    }
};

class JobInfo {

    std::map<const NodeInfo, Job*, cmp_node> nodes_address;
    std::map<const Job*, NodeInfo> nodes_job;

public:

    bool add(Job*, NodeInfo);
    bool remove(Job*);
    bool remove(NodeInfo);
    Job* get(NodeInfo);
    Job* get(long);
    NodeInfo getNode(long);
    NodeInfo get(Job*);
    void clear();
};


#endif //BANKOR_JOBINFO_H
