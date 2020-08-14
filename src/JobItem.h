//
// Created by akgunduz on 26.10.2015.
//

#ifndef DICON_JOBITEM_H
#define DICON_JOBITEM_H

#include "SchedulerItem.h"
#include "FileItem.h"
#include "JsonType.h"
#include "ProcessItem.h"
#include "ParameterItem.h"
#include "JobStatus.h"

#define MAX_CONTENT_TYPE 10
#define JOB_FILE "Job.json"

enum JOB_PATH {
    JOBPATH_ZIP,
    JOBPATH_DIR,
    JOBPATH_INVALID,
    JOBPATH_MAX,
};

class JobItem : public FileItem {

protected:

    char jobName[NAME_MAX]{};

    std::mutex mutex;

    std::vector<ContentItem *> contentList[MAX_CONTENT_TYPE];

    std::map<int, JsonType *> contentTypes;

    long duration{};

    JOB_STATUS status{JOBSTATUS_OK};

    std::vector<long> errorFileIDList{};

public:

    static long jobID;

    JobItem(const HostUnit&, const char*, long);
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

    ProcessItem* assignNode(ComponentUnit *);

    int getByOutput(int) const;
    bool setProcessIDByOutput(long, long);
    bool setProcessStateByFile(std::vector<long>&);
    JOB_STATUS createDependencyMap(std::vector<long>&);
    int updateDependency(long, int &);

    JOB_PATH checkPath(const  char*);
    bool extract(const char*, long&);

    bool check() override;

    JOB_STATUS getStatus();
};


#endif //DICON_JOBITEM_H
