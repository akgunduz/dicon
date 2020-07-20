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
#define JOB_FILE "Job.json"

class JobItem : public FileItem {

protected:

    char name[NAME_MAX]{};

    std::vector<ContentItem *> contentList[MAX_CONTENT_TYPE];

    std::map<int, JsonType *> contentTypes;

public:

    JobItem(const ComponentObject&, const char*, const char*);
    ~JobItem() override;
    ContentItem* getContent(int type, int index) const;
    int getContentCount(int type) const;
    void reset();
    bool parse();

    static bool parseNameNode(JobItem*, json_object *node);
    static bool parseFileNode(JobItem*, json_object *node);
    static bool parseParamNode(JobItem*, json_object *node);
    static bool parseProcessNode(JobItem *parent, json_object *node);

    const char* getName();
    void setName(const char*);

    int getProcessCount() const;
    ProcessItem* getProcess(int) const;

    int getFileCount() const;
    FileItem* getFile(int) const;
};


#endif //DICON_JSONFILE_H
