//
// Created by Haluk AKGUNDUZ on 21/03/18.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_JOBINFO_H
#define BANKOR_JOBINFO_H


//#include "Job.h"


#include "NodeInfo.h"
#include "ExecutorItem.h"

class JobInfo {

    std::map<const NodeInfo, ExecutorItem*, cmp_node> nodes_address;
    std::map<const ExecutorItem*, NodeInfo> nodes_job;

public:

    bool add(ExecutorItem*, NodeInfo);
    bool remove(ExecutorItem*);
    bool remove(NodeInfo);
    ExecutorItem* get(NodeInfo);
    ExecutorItem* get(long);
    NodeInfo getNode(long);
    NodeInfo get(ExecutorItem*);
    void clear();
};


#endif //BANKOR_JOBINFO_H
