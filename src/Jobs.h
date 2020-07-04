//
// Created by akgunduz on 19.03.2018.
//

#ifndef DICON_JOBS_H
#define DICON_JOBS_H


#include "Job.h"

typedef std::map<std::string, Job*, char_cmp> TypeJobList;


class Jobs {

    TypeJobList jobs;

public:

    Jobs();
    ~Jobs();
    bool add(Job*);
    bool add(ComponentObject , const char*);
    bool addPath(ComponentObject, bool = false);

    Job* get(const char*);
    Job* get(int);

    bool clear();

    bool isEmpty();
    unsigned long getCount();

};


#endif //DICON_JOBS_H
