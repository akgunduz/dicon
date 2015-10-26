//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"

Job::Job(Unit host, Unit node, const char *rootPath, FileContent* fileContent)
        : JsonFile(host, node, rootPath) {

    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "rules", this, parseRuleNode);

    if (fileContent == nullptr) {
        content = new FileContent(host, node, rootPath, JOB_FILE, FILE_JOB);
        if (!content->isValid()) {
            LOG_E("Can not read json file : %s!!!", JOB_FILE);
            return;
        }

    } else {
        content = fileContent;
    }

    content->setFlaggedToSent(true);

    char jobFilePath[PATH_MAX];
    sprintf(jobFilePath, "%s/%s", rootPath, JOB_FILE);
    if (!parse(jobFilePath)) {
        LOG_E("Job could not parsed!!!");
    }
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
        json_object *repeatNode = json_object_array_get_idx(child, 1);

        const char *path = json_object_get_string(pathNode);
        bool active = (bool) json_object_get_boolean(activeNode);
        int repeat = json_object_get_int(repeatNode);

        RuleItem *ruleItem = new RuleItem(path, active, repeat);
        ((Job*)parent)->rules.push_back(ruleItem);
    }

    return true;
}

Job::~Job() {

    for (std::list<RuleItem*>::iterator itr = rules.begin(); itr != rules.end(); itr++) {
        delete *itr;
    }

    rules.clear();
}

const char *Job::getFileName() {
    return JOB_FILE;
}

FILETYPE Job::getFileType() {
    return FILE_JOB;
}
