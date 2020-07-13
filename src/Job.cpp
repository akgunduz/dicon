//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"
#include "ParameterItem.h"

Job::Job(const ComponentObject& host, const char* jobDir)
        : JsonItem(host, jobDir, JOB_FILE){

    init();

}

Job::~Job() = default;

void Job::init() {

    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_EXECUTOR] = new JsonType(CONTENT_EXECUTOR, "executors", this, parseExecutorNode);

    if (!parse()) {
        LOGS_E(getHost(), "Job could not parsed!!!");
        return;
    }

    for (int i = 0; i < getExecutorCount(); i++) {
        getExecutor(i)->parse(this);
    }

    if (!createDependencyMap()){
        LOGS_E(getHost(), "Dependency Loop Detected in the Job!!!");
        return;
    }
}

bool Job::parseNameNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Name Node");
        return false;
    }

    const char *name = json_object_get_string(node);

    ((Job*)parent)->setName(name);

    return true;
}

bool Job::parseFileNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Files Node");
        return false;
    }

    for (int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Files Node");
            return false;
        }

        const char* path = json_object_get_string(child);

        auto *content = new FileItem(((Job*)parent)->getHost(),
                                     ((Job*)parent)->getJobDir(), path, 0, i);

        content->validate();

        ((Job*)parent)->contentList[CONTENT_FILE].push_back(content);

    }
    return true;
}

bool Job::parseParamNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Parameter Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Parameter Node");
            return false;
        }

        const char *param = json_object_get_string(child);

        auto *content = new ParameterItem(param);

        ((Job*)parent)->contentList[CONTENT_PARAM].push_back(content);
    }

    return true;
}

bool Job::parseExecutorNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Executor Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Executor Node");
            return false;
        }

        const char *exec = json_object_get_string(child);

        auto *content = new ProcessItem(exec);

        ((Job*)parent)->contentList[CONTENT_EXECUTOR].push_back(content);
    }

    return true;
}

const char *Job::getName() {

    return name;
}

void Job::setName(const char *_name) {

    strncpy(this->name, _name, JOB_MAX_NAME);
}

int Job::getExecutorCount() {

    return getContentCount(CONTENT_EXECUTOR);
}

ProcessItem* Job::getExecutor(int index) {

    return (ProcessItem*)getContent(CONTENT_EXECUTOR, index);
}

int Job::getFileCount() {

    return getContentCount(CONTENT_FILE);
}

FileItem* Job::getFile(int index) {

    return (FileItem*)getContent(CONTENT_FILE, index);
}
