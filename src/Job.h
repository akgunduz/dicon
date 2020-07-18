//
// Created by akgunduz on 23.10.2015.
//

#ifndef DICON_JOB_H
#define DICON_JOB_H

#include "JobItem.h"
#include "Md5.h"
#include "ProcessItem.h"
#include "ProcessInfo.h"
#include "miniz/miniz.h"

#define MAX_JOB_COUNT 100

class Job {

    ComponentObject host;

    JobItem *jobFile;

    std::mutex mutex;

    std::vector<ProcessInfo> processList;

public:

    Job(const ComponentObject&, const char*, const char*);

    ~Job();

    bool extract(const char*, const char*, char*);

    ComponentObject& getHost();

    int getFileCount();
    int getExecutorCount();
    const char* getName();
    FileItem* getFile(int);

    int getProcessCount() const;
    const ProcessInfo& getProcess(int) const;
    const ProcessInfo& assignNode(ComponentObject &);

    ProcessItem* getByOutput(int);
    bool createDependencyMap();
    int updateDependency(int, int &);
};


#endif //DICON_JOB_H
