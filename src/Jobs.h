//
// Created by akgunduz on 19.03.2018.
//

#ifndef BANKOR_JOBS_H
#define BANKOR_JOBS_H


#include "Job.h"
#include "JobInfo.h"


struct char_cmp {
    bool operator () (const char *a,const char *b) const
    {
        return strcmp(a,b) < 0;
    }
};

typedef std::map<const char *, Job*, char_cmp> TypeJobList;


class Jobs {

    TypeJobList jobs;

   // std::vector<Job*>* getJobs();

public:

    Jobs();
    ~Jobs();
    bool add(Job*);
    bool add(COMPONENT , const char*);
    //bool addList(Jobs*, bool = false);
    bool addList(COMPONENT, const char*, bool = false);

    Job* get(const char*);
    Job* get(int);
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
