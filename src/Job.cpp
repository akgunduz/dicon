//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"

Job::Job(const char *rootPath, const char* jobDir)
        : JsonItem(getJobPath(rootPath, jobDir).c_str()){

    init(jobDir);

}
/*
Job::Job(FileItem *fileItem, const char* jobDir)
        : JsonItem(fileItem) {

    init(jobDir);

    if (!parse(getAbsPath().c_str())) {
        LOG_E("Job could not parsed!!!");
    }
}
*/
Job::Job(const char *rootPath, const char* filePath, const char* jobDir)
        : JsonItem(getJobPath(rootPath, jobDir).c_str(), filePath, FILE_JOB) {

    init(jobDir);

    if (!parse(getAbsPath().c_str())) {
        LOG_E("Job could not parsed!!!");
    }
}

Job::~Job() {

}

void Job::init(const char* jobDir) {

    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "rules", this, parseRuleNode);
    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_ID] = new JsonType(CONTENT_ID, "id", this, parseIDNode);

    strcpy(this->jobDir, jobDir);
    this->attachedNode = 0;
}

bool Job::parseNameNode(void *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOG_E("Invalid JSON Name Node");
        return false;
    }

    const char *name = json_object_get_string(node);

    ((Job*)parent)->setName(name);

    return true;
}

bool Job::parseIDNode(void *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_int) {
        LOG_E("Invalid JSON Name Node");
        return false;
    }

    long id = json_object_get_int64(node);

    ((Job*)parent)->setID(id);

    return true;
}

bool Job::parseRuleNode(void *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOG_E("Invalid JSON Rules Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_array) {
            LOG_E("Invalid JSON Rules Node");
            return false;
        }

        if (json_object_array_length(child) != 3) {
            LOG_E("Invalid JSON Rules Node");
            return false;
        }

        json_object *pathNode = json_object_array_get_idx(child, 0);
        json_object *activeNode = json_object_array_get_idx(child, 1);
        json_object *repeatNode = json_object_array_get_idx(child, 2);

        const char *path = json_object_get_string(pathNode);
        bool active = (bool) json_object_get_boolean(activeNode);
        int repeat = json_object_get_int(repeatNode);

        Job* job = (Job*) parent;

        Rule *rule = new Rule(job->getRootPath(), path);
        rule->setActive(active);
        rule->setRepeat(repeat);

        job->contentList[CONTENT_FILE].push_back(rule);
    }

    return true;
}

const char *Job::getName() {
    return name;
}

void Job::setName(const char *name) {
    strncpy(this->name, name, 50);
}

std::string Job::getJobPath(const char *rootPath, const char *jobDir) {

    return std::string(rootPath) + "/" + (char*)jobDir;
}

std::string Job::getJobPath(const char *rootPath) {

    return std::string(rootPath) + "/Job_" + std::to_string(getID());
}

const char* Job::getJobDir() {

    return jobDir;
    //return std::string("Job_") + std::to_string(getID());
}

long Job::getAttachedNode() {
    return attachedNode;
}

void Job::setAttachedNode(long address) {
    this->attachedNode = address;
}

Rule *Job::getRule(int index) {
    return (Rule*)getContent(CONTENT_FILE, index);
}

int Job::getRuleCount() {
    return getContentCount(CONTENT_FILE);
}
