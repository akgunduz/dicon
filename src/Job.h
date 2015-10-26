//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_JOB_H
#define BANKOR_JOB_H

#include "Rule.h"
#include "RuleItem.h"
#include "JsonFile.h"

#define MAX_JOB_COUNT 100
#define JOB_FILE "Job.json"

class Job : public JsonFile {

    long id;
    char name[50];
    std::list<RuleItem*> rules;
    long attachedNode;

public:
    Job(Unit host, Unit node, const char* path, FileContent* fileContent = nullptr);
    ~Job();
    static bool parseRuleNode(void*, json_object *node);

    virtual const char *getFileName();

    virtual FILETYPE getFileType();
};


#endif //BANKOR_JOB_H
