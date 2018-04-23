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

    for (int i = 0; i < getContentCount(CONTENT_EXECUTOR); i++) {
        auto *item = (ExecutorItem*) getContent(CONTENT_EXECUTOR, i);
        item->parse(this);
    }

    if (!createDependencyMap()){
        LOG_E("Dependency Loop Detected in the Job!!!");
        return;
    }

    servedIndicator = 0;
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

        auto *content = new FileItem(job->getHost(), job->getJobDir(), path, i, is_dependent);

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

        auto *content = new ExecutorItem(exec);
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
//
//ExecutorItem* Job::getByAddress(long address) {
//
//    return nodes.get(address);
//}

ExecutorItem *Job::getByOutput(int index) {

    for (int i = 0; i < getCount(); i++) {

        auto *content = getByIndex(i);
        if (content->getOutputFile() == NULL) {
            continue;
        }

        if (content->getOutputFile()->getID() == index) {
            return content;
        }
    }

    return NULL;
}

ExecutorItem* Job::getUnServed() {

    if (orderedList.size() <= servedIndicator) {
        return NULL;
    }

    return orderedList[servedIndicator++];

//    int i = 0;
//    for (; i < getCount(); i++) {
//
//        long nodeAddress = nodes.get(getByIndex(i));
//        if (nodeAddress == 0) {
//            break;
//        }
//    }
//
//    if (i == getCount()) {
//        return NULL;
//    }
//
//    return getByIndex(i);
}


int Job::getOrderedCount() {

    return orderedList.size();
}

ExecutorItem *Job::getOrdered(int index) {

    return orderedList[index];
}

//bool Job::attachNode(ExecutorItem *item, long address) {
//
//    return nodes.add(item, address);
//}
//
//bool Job::detachNode(ExecutorItem *item) {
//
//    return nodes.remove(item);
//}
//
//void Job::resetNodes() {
//
//    nodes.clear();
//}

bool Job::createDependencyMap() {

    int depth[getContentCount(CONTENT_FILE)];
    std::vector<int> adj[getContentCount(CONTENT_FILE)];
    std::list<int> initial, final;

    bzero(depth, (size_t)getContentCount(CONTENT_FILE) * sizeof(int));

    for (int i = 0; i < getContentCount(CONTENT_EXECUTOR); i++) {

        auto *executor = (ExecutorItem*) getContent(CONTENT_EXECUTOR, i);

        for (int j = 0; j < executor->getDependentFileCount(); j++) {

            auto *file = executor->getDependentFile(j);

            int x = executor->getOutputFile()->getID();

            adj[file->getID()].push_back(x);

            depth[x]++;
        }
    }

    for (int i = 0; i < getContentCount(CONTENT_FILE); i++) {

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

    for (int i = 0; i < getContentCount(CONTENT_FILE); i++) {

        if (depth[i] > 0) {
            return false;
        }
    }

    for (int i : final) {

        auto *content = getByOutput(i);
        if (content) {
            orderedList.push_back(getByOutput(i));
        }
    }

    return true;
}


