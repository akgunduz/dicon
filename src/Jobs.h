//
// Created by akgunduz on 19.03.2018.
//

#ifndef BANKOR_JOBS_H
#define BANKOR_JOBS_H


#include "Job.h"
#include "JobInfo.h"


typedef std::map<std::string, Job*, char_cmp> TypeJobList;


class Jobs {

    TypeJobList jobs;

public:

    Jobs();
    ~Jobs();
    bool add(Job*);
    bool add(COMPONENT , const char*);
    bool addPath(COMPONENT, const char *, bool = false);

    Job* get(const char*);
    Job* get(int);

    bool clear();

    bool isEmpty();
    unsigned long getCount();

};


#endif //BANKOR_JOBS_H
