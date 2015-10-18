//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Rule.h"
#include "ParameterContent.h"
#include "ExecutorContent.h"

Rule::Rule(Unit host, Unit node, const char* rootPath, FileContent *fileContent) {

    this->unitHost = host;
    this->unitNode = node;
	strcpy(this->rootPath, rootPath);
	valid = false;
	parallel = false;

    if (fileContent == nullptr) {
        content = new FileContent(host, node, rootPath, RULE_FILE, FILE_RULE);
        if (!content->isValid()) {
            LOG_E("Can not read rule file %s!!!", RULE_FILE);
            return;
        }

    } else {
        content = fileContent;
    }

	char *ruleBuffer = nullptr;

	if (!readFile(content->getAbsPath(), &ruleBuffer)) {
		LOG_E("Read problem in rule file %s!!!", content->getAbsPath());
		return;
	}

	if (ruleBuffer == nullptr) {
		LOG_E("Read problem in rule file %s!!!", content->getAbsPath());
		return;
	}

	if (!parseBuffer(ruleBuffer)) {
		delete[] ruleBuffer;
		LOG_E("Could not parse rule file !!!");
		return;
	}

	delete[] ruleBuffer;

	content->setFlaggedToSent(true);

	valid = true;

}

Rule::~Rule() {

	if (content != nullptr) {
		delete content;
        content = nullptr;
	}

	for (int i = 0; i < RULE_MAX; i++) {
		for (int j = 0; j < contentList[i].size(); j++) {
			Content *item = contentList[i][j];
			delete item;
		}
	}
}

bool Rule::readFile(const char* path, char **pbuf) {

	int desc = open(path, O_RDONLY);
	if (desc == -1) {
		LOG_E("Rule File %s could not created or opened", path);
		return false;
	}

	LOG_T("Rule File is opened");

	off_t len = lseek(desc, 0, SEEK_END);
	if (len == -1) {
		LOG_E("Rule File : Error %d happened in lseek", errno);
		close(desc);
		return false;
	}

	lseek(desc, 0, SEEK_SET);

	*pbuf = new char[len];

	long count = read(desc, *pbuf, (size_t)len);
	if (count == -1) {
		LOG_E("Rule File : Error %d happened in read \"Rules.json\"", errno);
		delete[] *pbuf;
		close(desc);
		return false;
	}

	close(desc);

    return true;
}


bool Rule::parseRunTypeNode(json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_int) {
        LOG_E("Invalid JSON Files Node");
        return false;
    }

    const char *runType = json_object_get_string(node);

    if (strcmp(runType, "P") == 0 || strcmp(runType, "p") == 0) {
        parallel = true;
    }

    return true;
}

bool Rule::parseFileNode(json_object *node) {

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

		FileContent *content = new FileContent(unitHost, unitNode, getRootPath(), path, fileType);

		contentList[RULE_FILES].push_back(content);

	}
	return true;
}

bool Rule::parseParamNode(json_object *node) {

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
            contentList[RULE_PARAMETERS].push_back(content);
		}

	}

	return true;
}


bool Rule::parseExecutorNode(json_object *node) {

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
			contentList[RULE_EXECUTORS].push_back(content);
		}

	}

	return true;
}

bool Rule::parseBuffer(char *buf) {

	struct json_object* node = json_tokener_parse(buf);
	if (node == NULL){
		LOG_E("Invalid JSON Node");
		return false;
	}

	struct json_object* header;

	/*First check Rule header*/
	json_bool status = json_object_object_get_ex(node, RULE_HEADER, &header);
	if (status == 0){
		LOG_E("Invalid JSON Node");
		return false;
	}
	LOG_T("Rule header parsed");

	json_object_object_foreach(header, key, val) {

		std::map<std::string, RULE_TYPES>::iterator itr = ruleMap.find(key);
		if (itr == ruleMap.end()) {
			LOG_W("Unrecognized JSON Node, skipping to next");
			continue;

		} else {
			((this)->*(parserNodeList[itr->second]))(val);
		}

	}

	return true;
}

Content* Rule::getContent(RULE_TYPES type, int index) {
	return contentList[type][index];
}

int Rule::getContentCount(RULE_TYPES type) {
	return (int) contentList[type].size();
}

void Rule::reset() {
	for (int i = RULE_FILES; i < RULE_MAX; i++) {
		contentList[i].clear();
	}
}

void Rule::display() {
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
	}
}

FileContent *Rule::getContent() {
	return content;
}

int Rule::getFlaggedFileCount() {

	int count = 0;

	for (uint16_t i = 0; i < getContentCount(RULE_FILES); i++) {
		FileContent *content = (FileContent *)getContent(RULE_FILES, i);
		if (content->isFlaggedToSent()) {
			count++;
		}
	}

	return count;
}

const char* Rule::getRootPath() {
	return rootPath;
}

bool Rule::isParallel() {
	return parallel;
}
