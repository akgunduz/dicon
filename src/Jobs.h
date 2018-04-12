//
// Created by akgunduz on 19.03.2018.
//

#ifndef BANKOR_JOBS_H
#define BANKOR_JOBS_H


#include "Job.h"
#include "JobInfo.h"


class Jobs {

    std::vector<Job*> jobs;

    JobInfo nodes;

    std::vector<Job*>* getJobs();

public:

    Jobs();
    ~Jobs();
    bool addJob(Job*);
    bool addJob(COMPONENT , const char*);
    bool addJobList(Jobs*, bool = false);
    bool addJobList(COMPONENT, const char*, bool = false);

    Job* getJob(int);
//    Job* getJobByNode(NodeInfo);
//    Job* getJobByAddress(long);
//    NodeInfo getNodeByAddress(long);
//    Job* getJobUnServed();
//
//    bool attachNode(Job *job, NodeInfo node);
//    bool detachNode(Job *job);

//    bool reset();
    bool clear();

    bool isEmpty();
    unsigned long getCount();

};


#endif //BANKOR_JOBS_H
