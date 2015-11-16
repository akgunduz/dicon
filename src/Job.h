//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "Rule.h"
#include "JsonItem.h"
#include "Md5.h"
#include "FileList.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"

class AttachedNode {

public:
    long address;
    short id;
};


class Job : public JsonItem {

    long id;
    char name[50];
    AttachedNode attachedNode;

public:

  //  FileList *fileList;

  //  std::map<std::string, FileItem*> uniqueList;

    Job(FileItem *fileItem);
    Job(Unit host, const char *rootPath, const char* jobDir);
    Job(Unit host, const char *rootPath, const char* jobDir, const char* fileName);

    void init();
    ~Job();

    const char* getName();
    void setName(const char*);

    long getID();
    void setID(long id);

    AttachedNode getAttachedNode();
    void setAttachedNode(long address, short id);

    Rule* getRule(int index);
    int getRuleCount();

    static bool parseIDNode(void*, json_object *node);
    static bool parseNameNode(void*, json_object *node);
    static bool parseRuleNode(void*, json_object *node);

    FileList* prepareFileList(Unit host);
    FileList* prepareRuleList();
};


#endif //BANKOR_JOB_H
