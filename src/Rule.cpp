//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Rule.h"
#include "ParameterContent.h"
#include "ExecutorContent.h"

Rule::Rule(Unit host, Unit node, const char* rootPath, FileContent *fileContent)
        : JsonFile(host, node, rootPath) {

    contentTypes[CONTENT_RUNTYPE] = new JsonType(CONTENT_RUNTYPE, "runtype", this, parseRunTypeNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_EXECUTOR] = new JsonType(CONTENT_EXECUTOR, "executors", this, parseExecutorNode);

	parallel = false;

    if (fileContent == nullptr) {
        content = new FileContent(host, node, rootPath, RULE_FILE, FILE_RULE);
        if (!content->isValid()) {
            LOG_E("Can not read json file : %s!!!", RULE_FILE);
            return;
        }

    } else {
        content = fileContent;
    }

    content->setFlaggedToSent(true);

    char ruleFilePath[PATH_MAX];
    sprintf(ruleFilePath, "%s/%s", rootPath, RULE_FILE);
    if (!parse(ruleFilePath)) {
        LOG_E("Rule could not parsed!!!");
    }
}

Rule::~Rule() {

}

bool Rule::parseRunTypeNode(void *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_int) {
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

		FileContent *content = new FileContent(((Rule*)parent)->unitHost,
                                               ((Rule*)parent)->unitNode,
                                               ((Rule*)parent)->getRootPath(), path, fileType);

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

		ParameterContent *content = new ParameterContent(param);
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

		ExecutorContent *content = new ExecutorContent(exec);
		if (content->isValid()) {
            ((Rule*)parent)->contentList[CONTENT_EXECUTOR].push_back(content);
		}

	}

	return true;
}

void Rule::display() {
    /*
	for (int j = RULE_FILES; j < RULE_MAX; j++) {
		LOG_S("\t%s :", RuleTypes::getName((RULE_TYPES)j));
		for (int k = 0; k < getContentCount(static_cast<RULE_TYPES>(j)); k++) {
			Content *cnt = getContent(static_cast<RULE_TYPES>(j), k);

			switch(cnt->getType()) {
				case CONTENT_FILE: {
					FileContent *fcnt = (FileContent *)cnt;
					LOG_S("\t\tFile : %s", fcnt->getPath());
				}
					break;
				case CONTENT_PARAM: {
					ParameterContent *pcnt = (ParameterContent *)cnt;
					PARAM_TYPES type = pcnt->getParamType();
					switch(type) {
						case PARAM_LONG:
							LOG_S("\t\tParameter %ld with Type : %d", pcnt->getParam().latom, pcnt->getParamType());
							break;
						case PARAM_DOUBLE:
							LOG_S("\t\tParameter %f with Type : %d", pcnt->getParam().datom, pcnt->getParamType());
							break;
						case PARAM_STRING:
							LOG_S("\t\tParameter %s with Type : %d", pcnt->getParam().sPtr, pcnt->getParamType());
							break;
					}

				}
					break;
				case CONTENT_EXECUTOR: {
					ExecutorContent *ecnt = (ExecutorContent *) cnt;
					LOG_S("\t\tExecutor %s", ecnt->getExec().c_str());
				}
					break;
			}
		}
	}*/
}

const char *Rule::getFileName() {
    return RULE_FILE;
}

FILETYPE Rule::getFileType() {
    return FILE_RULE;
}

bool Rule::isParallel() {
	return parallel;
}
