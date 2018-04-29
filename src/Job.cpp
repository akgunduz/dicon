//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"
#include "ParameterItem.h"
#include "ExecutorItem.h"

Job::Job(COMPONENT host, const char* jobDir)
        : JsonItem(host, jobDir, JOB_FILE){

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
        return;
    }

    for (int i = 0; i < getExecutorCount(); i++) {
        getExecutor(i)->parse(this);
    }

    if (!createDependencyMap()){
        LOG_E("Dependency Loop Detected in the Job!!!");
        return;
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

        auto *job = (Job*) parent;

        bool is_dependent = strcmp(sFileType, "d") == 0;

        auto *content = new FileItem(job->getHost(), job->getJobDir(), path, i);

        content->validate();

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

        auto *content = new ParameterItem(param);

        ((Job*)parent)->contentList[CONTENT_PARAM].push_back(content);
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

        auto *content = new ExecutorItem(exec);

        ((Job*)parent)->contentList[CONTENT_EXECUTOR].push_back(content);
    }

    return true;
}

const char *Job::getName() {

    return name;
}

void Job::setName(const char *name) {

    strncpy(this->name, name, 50);
}

int Job::getExecutorCount() {

    return getContentCount(CONTENT_EXECUTOR);
}

ExecutorItem* Job::getExecutor(int index) {

    return (ExecutorItem*)getContent(CONTENT_EXECUTOR, index);
}

int Job::getFileCount() {

    return getContentCount(CONTENT_FILE);
}

FileItem* Job::getFile(int index) {

    return (FileItem*)getContent(CONTENT_FILE, index);
}

size_t Job::getOrderedCount() {

    return orderedList.size();
}

ExecutorItem *Job::getOrdered(int index) {

    return orderedList[index].first;
}

bool Job::getOrderedStatus(int index) {

    return orderedList[index].second;
}

void Job::setOrderedStatus(int index, bool status) {

    orderedList[index].second = status;
}

ExecutorItem* Job::getUnServed() {

    for (int i = 0; i < getOrderedCount(); i++) {

        if (getOrderedStatus(i)) {
            continue;
        }

        if (!getOrdered(i)->isValid()) {
            continue;
        }

        setOrderedStatus(i, true);

        return getOrdered(i);
    }

    return NULL;
}

int Job::getUnServedCount() {

    int count = 0;

    for (int i = 0; i < getOrderedCount(); i++) {

        if (getOrderedStatus(i)) {
            continue;
        }

        if (!getOrdered(i)->isValid()) {
            continue;
        }

        count++;
    }

    return count;
}

ExecutorItem *Job::getByOutput(int index) {

    for (int i = 0; i < getExecutorCount(); i++) {

        auto *executor = getExecutor(i);

        TypeFileInfoList list = FileInfo::getFileList(executor->getFileList(), true);
        if (list.empty()) {
            continue;
        }

        if (list[0].get()->getID() == index) {
            return executor;
        }
    }

    return NULL;
}

bool Job::createDependencyMap() {

    int depth[getFileCount()];
    std::vector<int> adj[getFileCount()];
    std::list<int> initial, final;

    bzero(depth, (size_t)getFileCount() * sizeof(int));

    TypeFileList outList, depList;

    for (int i = 0; i < getExecutorCount(); i++) {

        TypeFileInfoList *fileList = getExecutor(i)->getFileList();

        for (int j = 0; j < fileList->size(); j++) {

            FileItem* file = fileList->at(j).get();

            fileList->at(j).isOutput() ? outList.push_back(file) : depList.push_back(file);
        }

        for (int j = 0; j < depList.size(); j++) {

            //TODO will be updated with multi output files
            int id = outList[0]->getID();

            adj[depList[j]->getID()].push_back(id);

            depth[id]++;
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] == 0) {
            initial.push_back(i);
        }
    }

    while(!initial.empty()) {

        int current = initial.front();
        initial.pop_front();

        final.push_back(current);

        for (int i = 0; i < adj[current].size(); i++) {

            depth[adj[current][i]] -= 1;
            if (depth[adj[current][i]] == 0) {
                initial.push_back(adj[current][i]);
            }
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] > 0) {
            return false;
        }
    }

    for (int i : final) {

        auto *content = getByOutput(i);
        if (content) {
            orderedList.emplace_back(TypeExecutorProcess(content, false));
        }
    }

    return true;
}
