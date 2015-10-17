//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ExecutorContent.h"
#include "Rule.h"
#include "ParameterContent.h"

ExecutorContent::ExecutorContent(const char *line) {

	setValid(true);

	exec = line;
}

CONTENT_TYPES ExecutorContent::getType() {
	return CONTENT_EXECUTOR;
}

std::string ExecutorContent::getExec() {
	return exec;
}

void ExecutorContent::setExec(const std::string &exec) {
	this->exec = exec;
}


std::string ExecutorContent::getParsed(void *pRule) {

//	Rule *refRule = (Rule *)pRule;
	char parsed[PATH_MAX] = "";
	bool cmdMode = false;

	int cmdIndex = 0;
	RULE_TYPES cmdType = RULE_FILES;

	for (uint32_t i = 0; i < exec.size(); i++) {
		switch(exec[i]) {
			case '$':
				if (!cmdMode) {
					cmdMode = true;
					cmdIndex = 0;
					cmdType = RULE_MAX;
					break;
				}
				cmdMode = false;
				//no break
			case 'F':
			case 'f':
				if (cmdMode) {
					cmdType = RULE_FILES;
					break;
				}
				//no break
			case 'P':
			case 'p':
				if (cmdMode) {
					cmdType = RULE_PARAMETERS;
					break;
				}
				//no break
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (cmdMode) {
					cmdIndex = cmdIndex * 10 + (exec[i] - '0');
					break;
				}
				//no break
			case ' ':
				if (cmdMode) {
					cmdMode = false;
					parseCommand(parsed, pRule, cmdType, cmdIndex);
				}
				//no break
			default:
                sprintf(parsed, "%s%c", parsed, exec[i]);
				break;

		}

	}

	if (cmdMode) {
		parseCommand(parsed, pRule, cmdType, cmdIndex);
	}

	return parsed;
}

bool ExecutorContent::parseCommand(char *parsed, void *pRule, int cmdType, int cmdIndex) {

	Rule *refRule = (Rule *)pRule;

	if (cmdType == RULE_FILES) {
		FileContent *content = (FileContent *) refRule->getContent(RULE_FILES, cmdIndex);
		if (content != nullptr) {
            sprintf(parsed, "%s%s%s", parsed, refRule->getRootPath(), content->getPath());
		}

	} else if (cmdType == RULE_PARAMETERS) {
		ParameterContent *content = (ParameterContent *) refRule->getContent(RULE_PARAMETERS, cmdIndex);
		if (content != nullptr) {
			switch(content->getParamType()) {
				case PARAM_LONG:
                    sprintf(parsed, "%s%ld", parsed, content->getParam().latom);
                    break;
				case PARAM_DOUBLE:
                    sprintf(parsed, "%s%lf", parsed, content->getParam().datom);
					break;
				case PARAM_STRING:
                    strcat(parsed, content->getParam().sPtr);
					break;
			}
		}
	}

	return true;
}


