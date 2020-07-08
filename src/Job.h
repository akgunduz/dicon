//
// Created by akgunduz on 23.10.2015.
//

#ifndef DICON_JOB_H
#define DICON_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "ProcessItem.h"
#include "ProcessInfo.h"

#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"
#define JOB_MAX_NAME 50

class Job : public JsonItem {

    char name[JOB_MAX_NAME];

    std::mutex mutex;

    std::vector<ProcessInfo> processList;

public:

    Job(ComponentObject host, const char* jobDir);

    void init();
    ~Job() override;

    static bool parseNameNode(JsonItem*, json_object *node);
    static bool parseFileNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseExecutorNode(JsonItem*, json_object *node);

    const char* getName();
    void setName(const char*);

    int getExecutorCount();
    ProcessItem* getExecutor(int);

    int getFileCount();
    FileItem* getFile(int);

    int getProcessCount() const;
    int getProcessCount(PROCESS_STATE);
    const ProcessInfo& getProcess(int) const;
    const ProcessInfo& assignNode(ComponentObject &);
    void endProcess(int id);

    ProcessItem* getByOutput(int);
    bool createDependencyMap();
    int updateDependency();
};


#endif //DICON_JOB_H
