//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Rule.h"
#include "ParameterItem.h"
#include "ExecutorItem.h"

Rule::Rule(FileItem *fileItem)
        : JsonItem(fileItem) {

    init();
}

Rule::Rule(const char* rootPath, const char* filePath)
        : JsonItem(rootPath, filePath, FILE_RULE) {

    init();
}

void Rule::init() {

    contentTypes[CONTENT_RUNTYPE] = new JsonType(CONTENT_RUNTYPE, "runtype", this, parseRunTypeNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_EXECUTOR] = new JsonType(CONTENT_EXECUTOR, "executors", this, parseExecutorNode);

    parallel = false;
    active = true;
    repeat = 1;

    if (!parse(getAbsPath().c_str())) {
        LOG_E("Rule could not parsed!!!");
    }
}

Rule::~Rule() {

}

bool Rule::parseRunTypeNode(void *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOG_E("Invalid JSON Files Node");
        return false;
    }

    const char *runType = json_object_get_string(node);

    if (strcmp(runType, "P") == 0 || strcmp(runType, "p") == 0) {
        ((Rule*)parent)->parallel = true;
    }

    return true;
}

bool Rule::parseFileNode(void *parent, json_object *node) {

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

		json_object *pathNode = json_object_array_get_idx(child, 0);
		json_object *typeNode = json_object_array_get_idx(child, 1);

		const char* path = json_object_get_string(pathNode);
		const char* sFileType = json_object_get_string(typeNode);

        FILETYPE fileType = strcmp(sFileType, "c") == 0 ? FILE_COMMON : FILE_ARCH;

		FileItem *content = new FileItem(((Rule*)parent)->getRootPath(), path, fileType);

        ((Rule*)parent)->contentList[CONTENT_FILE].push_back(content);

	}
	return true;
}

bool Rule::parseParamNode(void *parent, json_object *node) {

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
            ((Rule*)parent)->contentList[CONTENT_PARAM].push_back(content);
		}

	}

	return true;
}


bool Rule::parseExecutorNode(void *parent, json_object *node) {

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
            ((Rule*)parent)->contentList[CONTENT_EXECUTOR].push_back(content);
		}

	}

	return true;
}

bool Rule::isParallel() {
	return parallel;
}

bool Rule::getActive() {
    return active;
}

void Rule::setActive(bool active) {
    this->active = active;
}

int Rule::getRepeat() {
    return repeat;
}

void Rule::setRepeat(int repeat) {
    this->repeat = repeat;
}


