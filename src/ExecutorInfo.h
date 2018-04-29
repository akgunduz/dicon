//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#ifndef BANKOR_EXECUTORINFO_H
#define BANKOR_EXECUTORINFO_H


#include "ExecutorItem.h"

class ExecutorInfo {

    ExecutorItem *item;
    long id;
    bool is_processed;

public:

    ExecutorInfo(long, ExecutorItem *);
    ExecutorItem* get();
    long getID();
    bool isProcessed();
    void setProcessState(bool);
};


#endif //BANKOR_EXECUTORINFO_H
