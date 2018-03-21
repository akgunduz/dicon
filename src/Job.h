//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "Rule.h"
#include "JsonItem.h"
#include "Md5.h"
#include "FileList.h"
#include "NodeInfo.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"

class Job : public JsonItem {

    char name[50];

public:

    Job(FileItem *fileItem);
    Job(Unit host, const char* jobDir);
    Job(Unit host, const char* jobDir, const char* fileName);

    void init();
    ~Job();

    const char* getName();
    void setName(const char*);

    Rule* getRule(int index);
    int getRuleCount();

    static bool parseNameNode(void*, json_object *node);
    static bool parseRuleNode(void*, json_object *node);

    FileList* prepareFileList(Unit host);
    FileList* prepareRuleList();
};


#endif //BANKOR_JOB_H
