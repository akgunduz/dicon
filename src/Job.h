//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "JsonItem.h"
#include "Md5.h"
#include "FileList.h"
#include "ExecutorItem.h"
#include "JobInfo.h"


#define MAX_JOB_COUNT 100
#define JOB_ITEM 0
#define JOB_FILE "Job.json"
#define JOB_DIR_PREFIX "Job_"

typedef std::map<const long, ExecutorItem*> TypeAddressExecList;
typedef std::map<const ExecutorItem*, long> TypeExecAddressList;

class Job : public JsonItem {

    char name[50];
	bool parallel;
    bool active;
    int repeat;

    JobInfo nodes;

public:

   // Job(FileItem *fileItem);
    Job(COMPONENT host, const char* jobDir);
    Job(COMPONENT host, const char* jobDir, const char* fileName);

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

    static bool parseNameNode(JsonItem*, json_object *node);
  //  static bool parseRuleNode(void*, json_object *node);
    static bool parseConcurrencyNode(JsonItem*, json_object *node);
    static bool parseFileNode(JsonItem*, json_object *node);
    static bool parseParamNode(JsonItem*, json_object *node);
    static bool parseExecutorNode(JsonItem*, json_object *node);

//    FileList* prepareFileList(ARCH = ARCH_MAX);
//    FileList* prepareRuleList();

    ExecutorItem* get(int);
    int getCount();
    ExecutorItem* get(long);
    long getNodeByAddress(long);
    ExecutorItem* getUnServed();

    bool attachNode(ExecutorItem*, long);
    bool detachNode(ExecutorItem*);

    bool resetNodes();
};


#endif //BANKOR_JOB_H
