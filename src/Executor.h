//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#ifndef BANKOR_EXECUTOR_H
#define BANKOR_EXECUTOR_H


#include "ExecutorItem.h"

class Executor {

    ExecutorItem *content;
    bool dependent;

public:

    bool isDependent();
};


#endif //BANKOR_EXECUTOR_H
