//
// Created by akgunduz on 26.10.2015.
//

#ifndef DICON_JOBITEM_H
#define DICON_JOBITEM_H

#include <mutex>
#include "nlohmann/json.hpp"

#include "ProcessItem.h"
#include "ParameterItem.h"
#include "JobStatus.h"

#define MAX_CONTENT_TYPE 10
#define JOB_FILE "Job.json"
#define JOB_SIGN "Job"

enum JOB_PATH {
    JOBPATH_ZIP,
    JOBPATH_DIR,
    JOBPATH_INVALID,
    JOBPATH_MAX,
};

class JobItem;

typedef bool (JobItem::*TypeJobParser)(const nlohmann::json&);

class JobItem : public FileItem {

protected:

    std::string jobName;

    std::mutex mutex;

    TypeContentList contentList[MAX_CONTENT_TYPE];

    std::map<std::string, TypeJobParser> contentParser;

    JOB_STATUS status{JOBSTATUS_OK};

    std::vector<long> errorFileIDList{};

public:

    inline static long jobID = 1;

    JobItem(const TypeHostUnit&, const std::filesystem::path&, long);
    ~JobItem() override;
    TypeContentItem getContent(int type, int index);
    int getContentCount(int type);
    void reset();
    bool parse();

    bool parseNameNode(const nlohmann::json&);
    bool parseFileNode(const nlohmann::json&);
    bool parseParamNode(const nlohmann::json&);
    bool parseProcessNode(const nlohmann::json&);

    const std::string& getJobName();
    void setJobName(const std::string&);

    int getProcessCount();
    int getProcessCount(PROCESS_STATE);
    TypeProcessItem getProcess(int);
    TypeProcessItem getProcessByID(long);

    int getFileCount();
    TypeFileItem getFile(int);
    TypeFileItem getFileByID(long);

    int getParameterCount();
    TypeParameterItem getParameter(int);

    TypeProcessItem assignNode(long);
    TypeProcessItem reAssignNode(long, long);

    int getByOutput(int);
    bool setProcessIDByOutput(long, long);
    bool setProcessStateByFile(std::vector<long>&);
    JOB_STATUS createDependencyMap(std::vector<long>&);
    int updateDependency(long, int &);

    JOB_PATH checkPath(const std::filesystem::path&);
    bool extract(const std::string&, long&);

    bool checkContent() override;

    JOB_STATUS getStatus();
};

typedef std::shared_ptr<JobItem> TypeJobItem;
typedef std::vector<TypeJobItem> TypeJobList;

#endif //DICON_JOBITEM_H
