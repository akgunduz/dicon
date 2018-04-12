//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"
#include "MapItem.h"
#include "ParameterItem.h"
#include "ExecutorItem.h"

//Job::Job(FileItem *fileItem)
//    : JsonItem (fileItem) {
//
//    init("");
//}

Job::Job(Unit host, const char* jobDir)
        : JsonItem(host, jobDir, JOB_FILE, FILE_JOB){

    init(jobDir);

}

Job::Job(Unit host, const char* jobDir, const char* fileName)
        : JsonItem(host, jobDir, fileName, FILE_JOB) {

    init(jobDir);

}

Job::~Job() {

}

void Job::init(const char* jobDir) {

    strcpy(this->jobDir, jobDir);

    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_CONCURRENCY] = new JsonType(CONTENT_CONCURRENCY, "concurrency", this, parseConcurrencyNode);
    contentTypes[CONTENT_MAP] = new JsonType(CONTENT_MAP, "files", this, parseMapNode);
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

bool Job::parseConcurrencyNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOG_E("Invalid JSON Files Node");
        return false;
    }

    const char *runType = json_object_get_string(node);

    if (strcmp(runType, "P") == 0 || strcmp(runType, "p") == 0) {
        ((Job*)parent)->parallel = true;
    }

    return true;
}

bool Job::parseMapNode(JsonItem *parent, json_object *node) {

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

        FILETYPE fileType = strcmp(sFileType, "c") == 0 ? FILE_COMMON : FILE_ARCH;

        Job* job = (Job*) parent;

        MapItem *content = new MapItem(job->getHost(), job->getJobDir(), path, fileType);

        job->contentList[CONTENT_MAP].push_back(content);

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

const char* Job::getJobDir() {
    return jobDir;
}

bool Job::isParallel() {
    return parallel;
}

bool Job::getActive() {
    return active;
}

void Job::setActive(bool active) {
    this->active = active;
}

int Job::getRepeat() {
    return repeat;
}

void Job::setRepeat(int repeat) {
    this->repeat = repeat;
}


//FileList* Job::prepareFileList(ARCH arch) {
//
//    FileList *fileList = new FileList(getJobDir());
//    fileList->set(this);
//
//    for (int i = 0; i < getContentCount(CONTENT_MAP); i++) {
//        MapItem *content = (MapItem *)getContent(CONTENT_MAP, i);
//        FileItem *fileItem = content->get(arch);
//        if (fileItem->isValid()) {
//            fileList->set(fileItem);
//        }
//    }
//
//    return fileList;
//}
//
//FileList* Job::prepareRuleList() {
//
//    FileList *fileList = new FileList(getJobDir());
//
//    fileList->set(this);
//
//    return fileList;
//}

ExecutorItem* Job::get(int index) {

    return (ExecutorItem*)getContent(CONTENT_EXECUTOR, index);
}

int Job::getCount() {

    return getContentCount(CONTENT_EXECUTOR);
}

ExecutorItem* Job::getByNode(NodeInfo node) {

    return nodes.get(node);
}

ExecutorItem* Job::getByAddress(long address) {

    return nodes.get(address);
}

NodeInfo Job::getNodeByAddress(long address) {

    return nodes.getNode(address);
}

ExecutorItem* Job::getUnServed() {

    int i = 0;
    for (; i < getCount(); i++) {

        NodeInfo node = nodes.get(get(i));
        if (node.getAddress() == 0) {
            break;
        }
    }

    if (i == getCount()) {
        return NULL;
    }

    return get(i);
}


bool Job::attachNode(ExecutorItem *item, NodeInfo node) {

    return nodes.add(item, node);
}

bool Job::detachNode(ExecutorItem *item) {

    return nodes.remove(item);
}

bool Job::resetNodes() {

    nodes.clear();
    return true;
}

