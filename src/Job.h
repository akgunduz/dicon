//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "Rule.h"
#include "JsonItem.h"

#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"

class Job : public JsonItem {

    char name[50];
    char jobDir[PATH_MAX];
    long attachedNode;

public:
    Job(const char *rootPath, const char* jobDir);
 //   Job(FileItem *fileItem, const char* jobDir);
    Job(const char* rootPath, const char* filePath, const char* jobDir);
    void init(const char* jobDir);
    ~Job();

    const char* getName();
    void setName(const char*);

    long getAttachedNode();
    void setAttachedNode(long address);

    std::string getJobPath(const char* rootPath, const char *jobDir);
    std::string getJobPath(const char* rootPath);
    const char* getJobDir();
    Rule* getRule(int index);
    int getRuleCount();

    static bool parseIDNode(void*, json_object *node);
    static bool parseNameNode(void*, json_object *node);
    static bool parseRuleNode(void*, json_object *node);
};


#endif //BANKOR_JOB_H
