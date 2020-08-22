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

    TypeContentList contentList[MAX_CONTENT_TYPE];

    std::map<int, TypeJsonType> contentTypes;

    long duration{};

    JOB_STATUS status{JOBSTATUS_OK};

    std::vector<long> errorFileIDList{};

public:

    static long jobID;

    JobItem(const TypeHostUnit&, const std::filesystem::path&, long);
    ~JobItem() override;
    TypeContentItem getContent(int type, int index) const;
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
    TypeProcessItem getProcess(int) const;
    TypeProcessItem getProcessByID(long) const;

    int getFileCount() const;
    TypeFileItem getFile(int) const;
    TypeFileItem getFileByID(long) const;

    int getParameterCount() const;
    TypeParameterItem getParameter(int) const;

    TypeProcessItem assignNode(long);
    TypeProcessItem reAssignNode(long, long);

    int getByOutput(int) const;
    bool setProcessIDByOutput(long, long);
    bool setProcessStateByFile(std::vector<long>&);
    JOB_STATUS createDependencyMap(std::vector<long>&);
    int updateDependency(long, int &);

    JOB_PATH checkPath(const std::filesystem::path&);
    bool extract(const std::string&, long&);

    bool check() override;

    JOB_STATUS getStatus();
};

typedef std::shared_ptr<JobItem> TypeJobItem;
typedef std::vector<TypeJobItem> TypeJobList;

#endif //DICON_JOBITEM_H
