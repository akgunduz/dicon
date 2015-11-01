//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "Rule.h"
#include "JsonItem.h"
#include "Md5.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"

class Job : public JsonItem {

    char name[50];
    long attachedNode;

public:

    std::map<std::string, FileItem*> uniqueList;

    Job(FileItem *fileItem, bool parseFiles);
    Job(const char *rootPath, const char* jobDir, bool parseFiles);
    Job(const char *rootPath, const char* jobDir, const char* fileName, bool parseFiles);

    void init(bool parseFiles);
    ~Job();

    const char* getName();
    void setName(const char*);

    long getAttachedNode();
    void setAttachedNode(long address);

    Rule* getRule(int index);
    int getRuleCount();

    static bool parseNameNode(void*, json_object *node);
    static bool parseRuleNode(void*, json_object *node);

    bool prepareUniqueList(std::vector<Md5> *md5List);
};


#endif //BANKOR_JOB_H
