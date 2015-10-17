//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Rule.h"
#include "ParameterContent.h"
#include "ExecutorContent.h"

Rule::Rule(const std::string &rootpath, const std::string &path) {

	mRootPath = rootpath;
	mIsValid = false;
	mRunType = RUN_SEQUENTIAL;

	if (path.empty()) {
		return;
	}

	std::string abspath = rootpath + path;

	mRuleFile = new FileContent(rootpath, path, nullptr);
	if (!mRuleFile->isValid()) {
		LOG_E("Can not read rule file %s!!!", abspath.c_str());
		return;
	}

	char *ruleBuffer = nullptr;

	if (!readFile(abspath, &ruleBuffer)) {
		LOG_E("Read problem in rule file %s!!!", abspath.c_str());
		return;
	}

	if (ruleBuffer == nullptr) {
		LOG_E("Read problem in rule file %s!!!", abspath.c_str());
		return;
	}

	if (!parseBuffer(ruleBuffer)) {
		delete[] ruleBuffer;
		LOG_E("Could not parse rule file !!!");
		return;
	}

	delete[] ruleBuffer;

	mRuleFile->setFlaggedToSent(true);

	mIsValid = true;
	LOG_I("Rule instance is created!!!");

}

Rule::~Rule() {

	if (mRuleFile != nullptr) {
		delete mRuleFile;
		mRuleFile = nullptr;
	}

	for (int i = 0; i < RULE_MAX; i++) {
		for (int j = 0; j < mContentList[i].size(); j++) {
			Content *item = mContentList[i][j];
			delete item;
		}
	}
}

bool Rule::readFile(const std::string &path, char **pbuf) {

	int readdesc = open(path.c_str(), O_RDONLY);
	if (readdesc == -1) {
		LOG_E("Rule File %s could not created or opened", path.c_str());
		return false;
	}

	LOG_T("Rule File is opened");

	off_t len = lseek(readdesc, 0, SEEK_END);
	if (len == -1) {
		LOG_E("Rule File : Error %d happened in lseek", errno);
		close(readdesc);
		return false;
	}

	lseek(readdesc, 0, SEEK_SET);

	*pbuf = new char[len];

	long count = read(readdesc, *pbuf, (size_t)len);
	if (count == -1) {
		LOG_E("Rule File : Error %d happened in read \"Rules.json\"", errno);
		delete[] *pbuf;
		close(readdesc);
		return false;
	}

	close(readdesc);
	LOG_T("Rule file is read");

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

		json_object *pathnode = json_object_array_get_idx(child, 0);
		json_object *md5node = json_object_array_get_idx(child, 1);

		std::string path(json_object_get_string(pathnode));
		const char *md5 = json_object_get_string(md5node);

		FileContent *content = new FileContent(mRootPath, path, md5);

		mContentList[RULE_FILES].push_back(content);

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
			mContentList[RULE_PARAMETERS].push_back(content);
		}

	}

	return true;
}

bool Rule::parseRunTypeNode(json_object *node) {

	enum json_type type = json_object_get_type(node);
	if (type != json_type_int) {
		LOG_E("Invalid JSON Files Node");
		return false;
	}

	const char *runtype = json_object_get_string(node);
	if (strcmp(runtype, "S") == 0 || strcmp(runtype, "s") == 0) {
		mRunType = RUN_SEQUENTIAL;
	} else if (strcmp(runtype, "P") == 0 || strcmp(runtype, "p") == 0) {
		mRunType = RUN_PARALLEL;
	} else  {
		LOG_E("Invalid Run Type Value");
		return false;
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
			mContentList[RULE_EXECUTORS].push_back(content);
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

		std::map<std::string, RULE_TYPES>::iterator itr = mRuleMap.find(key);
		if (itr == mRuleMap.end()) {
			LOG_W("Unrecognized JSON Node, skipping to next");
			continue;

		} else {
			((this)->*(parserNodeList[itr->second]))(val);
		}

	}

	return true;
}

Content* Rule::getContent(RULE_TYPES type, int index) {
	return mContentList[type][index];
}

ssize_t Rule::getContentCount(RULE_TYPES type) {
	return mContentList[type].size();
}

bool Rule::addContent(RULE_TYPES types, Content *content) {
	mContentList[types].push_back(content);
	return true;
}

void Rule::reset() {
	for (int i = RULE_FILES; i < RULE_MAX; i++) {
		mContentList[i].clear();
	}
}

void Rule::display() {
	for (int j = RULE_FILES; j < RULE_MAX; j++) {
		LOG_S("\t%s :", sRuleTypes[j].c_str());
		for (int k = 0; k < getContentCount(static_cast<RULE_TYPES>(j)); k++) {
			Content *cnt = getContent(static_cast<RULE_TYPES>(j), k);

			switch(cnt->getType()) {
				case CONTENT_FILE: {
					FileContent *fcnt = (FileContent *)cnt;
					LOG_S("\t\tFile : %s", fcnt->getPath().c_str());
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

FileContent *Rule::getRuleFile() {
	return mRuleFile;
}

ssize_t Rule::getFlaggedFileCount() {

	ssize_t count = 0;

	for (uint16_t i = 0; i < getContentCount(RULE_FILES); i++) {
		FileContent *content = (FileContent *)getContent(RULE_FILES, i);
		if (content->isFlaggedToSent()) {
			count++;
		}
	}

	return count;
}

bool Rule::isValid() {

	return mIsValid;

}

bool Rule::updateFileContent(FileContent *refcontent) {

	for (uint16_t i = 0; i < getContentCount(RULE_FILES); i++) {
		FileContent *content = (FileContent *)getContent(RULE_FILES, i);
		if (refcontent->getPath().compare(content->getPath()) == 0) {
			content->set(refcontent);
		}
	}

	return true;
}

std::string Rule::getRootPath() {

	return mRootPath;

}

RUN_TYPE Rule::getRunType() {

	return mRunType;

}
