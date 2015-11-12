//
// Created by akgunduz on 23.10.2015.
//

#include <set>
#include "Job.h"
#include "MapItem.h"

Job::Job(FileItem *fileItem)
    : JsonItem (fileItem) {

    init();
}

Job::Job(Unit host, const char *rootPath, const char* jobDir)
        : JsonItem(host, rootPath, jobDir, JOB_FILE, FILE_JOB){

    init();

}

Job::Job(Unit host, const char *rootPath, const char* jobDir, const char* fileName)
        : JsonItem(host, rootPath, jobDir, fileName, FILE_JOB) {

    init();

}

Job::~Job() {

}

void Job::init() {

    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "rules", this, parseRuleNode);
    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);

    this->attachedNode.address = 0;

    if (!parse()) {
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

        Rule *rule = new Rule(job->getHost(), job->getRootPath(), job->getJobDir(), path);
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

AttachedNode Job::getAttachedNode() {
    return attachedNode;
}

void Job::setAttachedNode(long address, short id) {
    this->attachedNode.address = address;
    this->attachedNode.id = id;
}

Rule *Job::getRule(int index) {
    return (Rule*)getContent(CONTENT_FILE, index);
}

int Job::getRuleCount() {
    return getContentCount(CONTENT_FILE);
}

FileList* Job::prepareFileList(Unit host) {

    FileList *fileList = new FileList(getJobDir());

    for (int j = 0; j < getContentCount(CONTENT_FILE); j++) {
        Rule *rule = (Rule *)getContent(CONTENT_FILE, j);
        for (int i = 0; i < rule->getContentCount(CONTENT_MAP); i++) {
            MapItem *content = (MapItem *)rule->getContent(CONTENT_MAP, i);
            FileItem *fileItem = content->get(host.getID());
            if (fileItem->isValid()) {
                fileList->set(fileItem);
            }
        }
    }

    return fileList;
}

FileList* Job::prepareRuleList() {

    FileList *fileList = new FileList(getJobDir());

    fileList->set(this);

    for (int j = 0; j < getContentCount(CONTENT_FILE); j++) {
        Rule *rule = (Rule *)getContent(CONTENT_FILE, j);
        fileList->set(rule);
    }

    return fileList;
}