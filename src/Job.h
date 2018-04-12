//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "FileList.h"
#include "NodeInfo.h"
#include "ExecutorItem.h"
#include "JobInfo.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"

class Job : public JsonItem {

    char name[50];
	bool parallel;
    bool active;
    int repeat;

    JobInfo nodes;
    char jobDir[MAX_JOB_DIR_LENGTH];

public:

   // Job(FileItem *fileItem);
    Job(Unit host, const char* jobDir);
    Job(Unit host, const char* jobDir, const char* fileName);

    void init(const char*);
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

    static bool parseNameNode(JsonItem*, json_object *node);
  //  static bool parseRuleNode(void*, json_object *node);
    static bool parseConcurrencyNode(JsonItem*, json_object *node);
    static bool parseMapNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseExecutorNode(JsonItem*, json_object *node);

//    FileList* prepareFileList(ARCH = ARCH_MAX);
//    FileList* prepareRuleList();

    ExecutorItem* get(int);
    int getCount();
    const char* getJobDir();
    ExecutorItem* getByNode(NodeInfo);
    ExecutorItem* getByAddress(long);
    NodeInfo getNodeByAddress(long);
    ExecutorItem* getUnServed();

    bool attachNode(ExecutorItem*, NodeInfo node);
    bool detachNode(ExecutorItem*);

    bool resetNodes();
};


#endif //BANKOR_JOB_H
