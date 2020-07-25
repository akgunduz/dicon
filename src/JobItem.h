//
// Created by akgunduz on 26.10.2015.
//

#ifndef DICON_JSONFILE_H
#define DICON_JSONFILE_H

#include "SchedulerItem.h"
#include "FileItem.h"
#include "JsonType.h"
#include "ProcessItem.h"
#include "ParameterItem.h"

#define MAX_CONTENT_TYPE 10
#define JOB_FILE "Job.json"

class JobItem : public FileItem {

protected:

    char jobName[NAME_MAX]{};

    std::mutex mutex;

    std::vector<ContentItem *> contentList[MAX_CONTENT_TYPE];

    std::map<int, JsonType *> contentTypes;

    long duration{};

public:

    static long jobID;

    JobItem(const ComponentObject&, const char*, long);
    ~JobItem() override;
    ContentItem* getContent(int type, int index) const;
    int getContentCount(int type) const;
    void reset();
    bool parse();

    static bool parseNameNode(JobItem*, json_object *node);
    static bool parseFileNode(JobItem*, json_object *node);
    static bool parseParamNode(JobItem*, json_object *node);
    static bool parseProcessNode(JobItem *parent, json_object *node);

    const char* getJobName() const;
    void setJobName(const char*);

    long getDuration() const;
    void setDuration(long duration);

    int getProcessCount() const;
    int getProcessCount(PROCESS_STATE) const;
    ProcessItem* getProcess(int) const;
    ProcessItem* getProcessByID(long) const;

    int getFileCount() const;
    FileItem* getFile(int) const;
    FileItem* getFileByID(long) const;

    int getParameterCount() const;
    ParameterItem* getParameter(int) const;

    ProcessItem* assignNode(ComponentObject &);

    int getByOutput(int) const;
    bool createDependencyMap();
    int updateDependency(long, int &);

    bool extract(const char*, long&);

    bool check() override;
};


#endif //DICON_JSONFILE_H
