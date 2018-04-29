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

typedef std::pair<ExecutorItem *, bool> TypeExecutorProcess;

class Job : public JsonItem {

    char name[50];

    std::vector<TypeExecutorProcess> orderedList;
    //std::deque<int> independentList;

public:

    Job(COMPONENT host, const char* jobDir);

    void init();
    ~Job();

    static bool parseNameNode(JsonItem*, json_object *node);
    static bool parseFileNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseExecutorNode(JsonItem*, json_object *node);

    const char* getName();
    void setName(const char*);

    int getExecutorCount();
    ExecutorItem* getExecutor(int);

    int getFileCount();
    FileItem* getFile(int);

    size_t getOrderedCount();
    ExecutorItem* getOrdered(int);
    bool getOrderedStatus(int);
    void setOrderedStatus(int, bool);

    int getUnServedCount();
    ExecutorItem* getUnServed();

    ExecutorItem* getByOutput(int);
    bool createDependencyMap();
};


#endif //BANKOR_JOB_H
