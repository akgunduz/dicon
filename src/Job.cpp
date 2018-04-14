//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"
#include "ParameterItem.h"
#include "ExecutorItem.h"

//Job::Job(FileItem *fileItem)
//    : JsonItem (fileItem) {
//
//    init("");
//}

Job::Job(COMPONENT host, const char* jobDir)
        : JsonItem(host, jobDir, JOB_FILE){

    init();

}

Job::Job(COMPONENT host, const char* jobDir, const char* fileName)
        : JsonItem(host, jobDir, fileName) {

    init();

}

Job::~Job() {

}

void Job::init() {

    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_EXECUTOR] = new JsonType(CONTENT_EXECUTOR, "executors", this, parseExecutorNode);

    if (!parse()) {
        LOG_E("Job could not parsed!!!");
    }

    for (int i = 0; i < getContentCount(CONTENT_EXECUTOR); i++) {
        ExecutorItem *item = (ExecutorItem*) getContent(CONTENT_EXECUTOR, i);
        item->parse(this);
    }
}

bool Job::parseNameNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOG_E("Invalid JSON Name Node");
        return false;
    }

    const char *name = json_object_get_string(node);

    ((Job*)parent)->setName(name);

    return true;
}

bool Job::parseFileNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOG_E("Invalid JSON Files Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_array) {
            LOG_E("Invalid JSON Files Node");
            return false;
        }

        if (json_object_array_length(child) != 2) {
            LOG_E("Invalid JSON Files Node");
            return false;
        }

        const char* path = json_object_get_string(json_object_array_get_idx(child, 0));
        const char* sFileType = json_object_get_string(json_object_array_get_idx(child, 1));

        Job* job = (Job*) parent;

        FileItem *content = new FileItem(job->getHost(), job->getJobDir(), path);

        job->contentList[CONTENT_FILE].push_back(content);

    }
    return true;
}

bool Job::parseParamNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOG_E("Invalid JSON Parameter Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOG_E("Invalid JSON Parameter Node");
            return false;
        }

        const char *param = json_object_get_string(child);

        ParameterItem *content = new ParameterItem(param);
        if (content->isValid()) {
            ((Job*)parent)->contentList[CONTENT_PARAM].push_back(content);
        }

    }

    return true;
}

bool Job::parseExecutorNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOG_E("Invalid JSON Executor Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOG_E("Invalid JSON Executor Node");
            return false;
        }

        const char *exec = json_object_get_string(child);

        ExecutorItem *content = new ExecutorItem(exec);
        if (content->isValid()) {
            ((Job*)parent)->contentList[CONTENT_EXECUTOR].push_back(content);
        }

    }

    return true;
}

const char *Job::getName() {

    return name;
}

void Job::setName(const char *name) {

    strncpy(this->name, name, 50);
}

ExecutorItem* Job::getByIndex(int index) {

    return (ExecutorItem*)getContent(CONTENT_EXECUTOR, index);
}

int Job::getCount() {

    return getContentCount(CONTENT_EXECUTOR);
}

ExecutorItem* Job::getByAddress(long address) {

    return nodes.get(address);
}

ExecutorItem* Job::getUnServed() {

    int i = 0;
    for (; i < getCount(); i++) {

        long nodeAddress = nodes.get(getByIndex(i));
        if (nodeAddress == 0) {
            break;
        }
    }

    if (i == getCount()) {
        return NULL;
    }

    return getByIndex(i);
}


bool Job::attachNode(ExecutorItem *item, long address) {

    return nodes.add(item, address);
}

bool Job::detachNode(ExecutorItem *item) {

    return nodes.remove(item);
}

void Job::resetNodes() {

    nodes.clear();
}

