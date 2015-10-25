//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H


#include "Rule.h"

class Job {
    char id[50];
    Rule* rule;
    long attachedNode;
    int repeat;
};


#endif //BANKOR_JOB_H
