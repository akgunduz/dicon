//
// Created by Haluk AKGUNDUZ on 21/03/18.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_JOBINFO_H
#define DICON_JOBINFO_H


//#include "Job.h"


#include "ExecutorItem.h"

typedef std::map<const long, ExecutorItem*> TypeAddressExecList;
typedef std::map<const ExecutorItem*, long> TypeExecAddressList;

class JobInfo {

    TypeAddressExecList nodes_address;
    TypeExecAddressList nodes_job;

public:

    bool add(ExecutorItem*, long);
    bool remove(ExecutorItem*);
    bool remove(long);
    ExecutorItem* get(long);
    long getNode(long);
    long get(ExecutorItem*);
    void clear();
};


#endif //DICON_JOBINFO_H
