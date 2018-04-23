//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "FileList.h"
#include "ExecutorItem.h"
#include "JobInfo.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"

typedef std::map<const long, ExecutorItem*> TypeAddressExecList;
typedef std::map<const ExecutorItem*, long> TypeExecAddressList;

class Job : public JsonItem {

    char name[50];
    JobInfo nodes;
    std::vector<ExecutorItem *> orderedList;
    std::vector<ExecutorItem *> independentList;
    int servedIndicator;

public:

    Job(COMPONENT host, const char* jobDir);

    void init();
    ~Job();

    const char* getName();
    void setName(const char*);

    static bool parseNameNode(JsonItem*, json_object *node);
    static bool parseFileNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseExecutorNode(JsonItem*, json_object *node);

    int getExecutorCount();
    ExecutorItem* getExecutor(int);
    ExecutorItem* getUnServed();
    ExecutorItem* getByOutput(int);

    ExecutorItem* getOrdered(int);
    size_t getOrderedCount();
    size_t getUnServedCount();

//    bool attachNode(ExecutorItem*, long);
//    bool detachNode(ExecutorItem*);
//    void resetNodes();

    bool createDependencyMap();
    bool createIndepentExecutions();
    bool updateIndependentExecutions(int);
};


#endif //BANKOR_JOB_H
