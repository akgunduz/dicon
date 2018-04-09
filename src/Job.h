//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "FileList.h"
#include "NodeInfo.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"

class Job : public JsonItem {

    char name[50];
	bool parallel;
    bool active;
    int repeat;

public:

    Job(FileItem *fileItem);
    Job(Unit host, const char* jobDir);
    Job(Unit host, const char* jobDir, const char* fileName);

    void init();
    ~Job();

    const char* getName();
    void setName(const char*);

//    Rule* getRule(int index);
//    int getRuleCount();

    bool isParallel();

    bool getActive();
    void setActive(bool active);

    int getRepeat();
    void setRepeat(int repeat);

    static bool parseNameNode(void*, json_object *node);
  //  static bool parseRuleNode(void*, json_object *node);
    static bool parseConcurrencyNode(void*, json_object *node);
    static bool parseMapNode(void*, json_object *node);
    static bool parseParamNode(void*, json_object *node);
    static bool parseExecutorNode(void*, json_object *node);

    FileList* prepareFileList(ARCH = ARCH_MAX);
    FileList* prepareRuleList();
};


#endif //BANKOR_JOB_H
