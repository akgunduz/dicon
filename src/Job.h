//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "ExecutorItem.h"
#include "JobInfo.h"
#include "ExecutorInfo.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"

class Job : public JsonItem {

    char name[50];
    TypeUUID id;

    std::vector<ExecutorInfo> orderedList;

public:

    Job(ComponentObject host, const char* jobDir);

    void init();
    ~Job();

    static bool parseNameNode(JsonItem*, json_object *node);
    static bool parseFileNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseExecutorNode(JsonItem*, json_object *node);

    TypeUUID& getJobID();

    const char* getName();
    void setName(const char*);

    int getExecutorCount();
    ExecutorItem* getExecutor(int);

    int getFileCount();
    FileItem* getFile(int);

    long getOrderedCount();
    long getProvisionCount();
    ExecutorInfo getOrdered(int);
    ExecutorItem* getOrderedExecution(int);
    PROCESS_STATE getOrderedState(int);
    void setOrderedState(int, PROCESS_STATE);

    long getUnServedCount();
    ExecutorInfo getUnServed();
    bool isEnded();

    ExecutorItem* getByOutput(int);
    bool createDependencyMap();
};


#endif //BANKOR_JOB_H
