//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "ProcessItem.h"
#include "ProcessInfo.h"
#include "Uuid.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"

class Job : public JsonItem {

private:

    char name[50];
    Uuid id;
    long unServedCount;

    std::vector<ProcessInfo> orderedList;

    std::mutex mutex;

    void init();

    static bool parseNameNode(JsonItem*, json_object *node);
    static bool parseFileNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseProcessNode(JsonItem *, json_object *node);

    const char* getName();
    void setName(const char*);

    ProcessInfo getOrdered(int);
    PROCESS_STATE getOrderedState(int);
    void setOrderedState(int, PROCESS_STATE);

    ProcessItem* getByOutput(int);
    bool createDependencyMap();

public:

    Job(ComponentObject host, const char* jobDir);

    ~Job();

    Uuid getJobID();

    int getFileCount();
    FileItem* getFile(int);

    int getProcessCount();
    ProcessItem* getProcess(int);

    long getOrderedCount();
    ProcessItem* getOrderedProcess(int);

    long getUnServedCount();
    ProcessInfo getUnServed();
    void updateUnServed(int = 0, PROCESS_STATE = PROCESS_STATE_MAX);
    bool isEnded();
};


#endif //BANKOR_JOB_H
