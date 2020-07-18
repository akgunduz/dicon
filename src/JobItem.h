//
// Created by akgunduz on 26.10.2015.
//

#ifndef DICON_JSONFILE_H
#define DICON_JSONFILE_H

#include "SchedulerItem.h"
#include "FileItem.h"
#include "JsonType.h"
#include "ProcessItem.h"

#define MAX_CONTENT_TYPE 10
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"
#define JOB_MAX_NAME 50

class JobItem : public FileItem {

protected:

    char name[NAME_MAX]{};

    std::vector<ContentItem *> contentList[MAX_CONTENT_TYPE];

    std::map<int, JsonType *> contentTypes;

public:

    JobItem(const ComponentObject& host);
    JobItem(FileItem *fileItem);
    JobItem(const ComponentObject&, const char*, const char*);
    virtual ~JobItem();
    bool init();
    ContentItem* getContent(int type, int index) const;
    int getContentCount(int type) const;
    void reset();
    bool parse();

    static bool parseNameNode(JobItem*, json_object *node);
    static bool parseFileNode(JobItem*, json_object *node);
    static bool parseParamNode(JobItem*, json_object *node);
    static bool parseExecutorNode(JobItem*, json_object *node);

    const char* getName();
    void setName(const char*);

    int getExecutorCount();
    ProcessItem* getExecutor(int);

    int getFileCount();
    FileItem* getFile(int);
};


#endif //DICON_JSONFILE_H
