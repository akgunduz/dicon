//
// Created by akgunduz on 26.10.2015.
//

#include "JobItem.h"
#include "ParameterItem.h"

JobItem::JobItem(const ComponentObject& host, const char* jobName, const char* fileName)
        : FileItem(host, jobName, fileName, 0, -1) {

    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_PROCESS] = new JsonType(CONTENT_PROCESS, "processes", this, parseProcessNode);

    if (!validate()) {
        LOGS_E(getHost(), "Job file could not validated!!!");
        return;
    }

    if (!parse()) {
        LOGS_E(getHost(), "Job file could not parsed!!!");
        return;
    }

    for (int i = 0; i < getProcessCount(); i++) {
        getProcess(i)->parse(this);
    }
}

JobItem::~JobItem() {

    for (auto & contentType : contentTypes) {
        delete contentType.second;
    }

    for (auto & content : contentList) {
        for (auto item : content) {
            delete item;
        }
    }
}

ContentItem* JobItem::getContent(int type, int index) const {

    return contentList[type][index];
}

int JobItem::getContentCount(int type) const {

    return (int) contentList[type].size();
}

void JobItem::reset() {

    for (int i = 0; i < contentList->size(); i++) {
        contentList[i].clear();
    }
}

bool JobItem::parse() {

    struct json_object* node = json_object_from_file(Util::getAbsRefPath(getHost().getRootPath(), getJobName(), getFileName()).c_str());
    if (node == nullptr){
        LOGS_E(getHost(), "Invalid JSON File");
        return false;
    }

    auto* header = (struct json_object*)json_object_get_object(node)->head->v;

    json_object_object_foreach(header, key, val) {

        for (auto & contentType : contentTypes) {

            JsonType* type = contentType.second;
            if (strcmp(type->name, key) == 0) {
                (type->parser)(type->parent, val);
                break;
            }
        }
    }

    return true;
}

bool JobItem::parseNameNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOGS_E(parent->getHost(), "Invalid JSON Name Node");
        return false;
    }

    const char *name = json_object_get_string(node);

    parent->setName(name);

    return true;
}

bool JobItem::parseFileNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(parent->getHost(), "Invalid JSON Files Node");
        return false;
    }

    for (int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(parent->getHost(), "Invalid JSON Files Node");
            return false;
        }

        const char* path = json_object_get_string(child);

        auto *content = new FileItem(parent->getHost(),
                                     parent->getName(), path, 0, i);

        content->validate();

        parent->contentList[CONTENT_FILE].push_back(content);

    }
    return true;
}

bool JobItem::parseParamNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(parent->getHost(), "Invalid JSON Parameter Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(parent->getHost(), "Invalid JSON Parameter Node");
            return false;
        }

        const char *param = json_object_get_string(child);

        auto *content = new ParameterItem(param);

        parent->contentList[CONTENT_PARAM].push_back(content);
    }

    return true;
}

bool JobItem::parseProcessNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(parent->getHost(), "Invalid JSON Process Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(parent->getHost(), "Invalid JSON Process Node");
            return false;
        }

        const char *process = json_object_get_string(child);

        auto *content = new ProcessItem(process);

        parent->contentList[CONTENT_PROCESS].push_back(content);
    }

    return true;
}

const char *JobItem::getName() {

    return name;
}

void JobItem::setName(const char *_name) {

    strncpy(this->name, _name, NAME_MAX);
}

int JobItem::getProcessCount() {

    return getContentCount(CONTENT_PROCESS);
}

ProcessItem* JobItem::getProcess(int index) {

    return (ProcessItem*)getContent(CONTENT_PROCESS, index);
}

int JobItem::getFileCount() {

    return getContentCount(CONTENT_FILE);
}

FileItem* JobItem::getFile(int index) {

    return (FileItem*)getContent(CONTENT_FILE, index);
}
