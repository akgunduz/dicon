//
// Created by akgunduz on 23.10.2015.
//

#include <set>
#include "Job.h"

Job::Job(FileItem *fileItem, bool parseFiles)
    : JsonItem (fileItem) {

    init(parseFiles);
}

Job::Job(const char *rootPath, const char* jobDir, bool parseFiles)
        : JsonItem(rootPath, jobDir, JOB_FILE, FILE_JOB){

    init(parseFiles);

}

Job::Job(const char *rootPath, const char* jobDir, const char* fileName, bool parseFiles)
        : JsonItem(rootPath, jobDir, fileName, FILE_JOB) {

    init(parseFiles);

}

Job::~Job() {

}

void Job::init(bool parseFiles) {

    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "rules", this, parseRuleNode);
    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);

    this->attachedNode = 0;

    if (parseFiles && !parse()) {
        LOG_E("Job could not parsed!!!");
    }
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

        Rule *rule = new Rule(job->getRootPath(), job->getJobDir(), path, true);
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

bool Job::prepareUniqueList(std::vector<Md5> *md5List) {

    for (int i = 0; i < getRuleCount(); i++) {
        Rule *rule = (Rule *)getContent(CONTENT_FILE, i);
        uniqueList[(char*)rule->getMD5()] = rule;
        for (int j = 0; j < rule->getContentCount(CONTENT_FILE); j++) {
            FileItem *fileItem = (FileItem *)rule->getContent(CONTENT_FILE, j);
            uniqueList[(char*)fileItem->getMD5()] = fileItem;
        }
    }

    for (int i = 0; i < md5List->size(); i++) {
        uniqueList.erase((char*)(*md5List)[i].data);
    }

    return true;
}
