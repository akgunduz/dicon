//
// Created by akgunduz on 19.03.2018.
//

#ifndef BANKOR_JOBS_H
#define BANKOR_JOBS_H


#include "Job.h"
#include "JobInfo.h"

typedef std::map<TypeUUID , Job*> TypeJobList;


class Jobs {

    TypeJobList jobs;

public:

    Jobs();
    ~Jobs();
    bool add(Job*);
    bool add(ComponentObject , const char *);
    bool addPath(ComponentObject, bool = false);

    Job* get(TypeUUID&);
    Job* get(int);

    bool clear();

    bool isEmpty();
    unsigned long getCount();

};


#endif //BANKOR_JOBS_H
