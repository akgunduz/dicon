//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ExecutorItem.h"
#include "Rule.h"
#include "ParameterItem.h"

ExecutorItem::ExecutorItem()
        : ContentItem () {

};

ExecutorItem::ExecutorItem(const char *line)
        : ContentItem () {

	setValid(true);

	exec = line;
}

CONTENT_TYPES ExecutorItem::getType() {
	return CONTENT_EXECUTOR;
}

std::string ExecutorItem::getExec() {
	return exec;
}

void ExecutorItem::setExec(const std::string &exec) {
	this->exec = exec;
}


std::string ExecutorItem::getParsed(void *pRule) {

//	Rule *refRule = (Rule *)pRule;
	char parsed[PATH_MAX] = "";
	bool cmdMode = false;

	int cmdIndex = 0;
	CONTENT_TYPES cmdType = CONTENT_FILE;

	for (uint32_t i = 0; i < exec.size(); i++) {
		switch(exec[i]) {
			case '$':
				if (!cmdMode) {
					cmdMode = true;
					cmdIndex = 0;
					cmdType = CONTENT_MAX;
					break;
				}
				cmdMode = false;
				//no break
			case 'F':
			case 'f':
				if (cmdMode) {
					cmdType = CONTENT_FILE;
					break;
				}
				//no break
			case 'P':
			case 'p':
				if (cmdMode) {
					cmdType = CONTENT_PARAM;
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

bool ExecutorItem::parseCommand(char *parsed, void *pRule, int cmdType, int cmdIndex) {

	Rule *refRule = (Rule *)pRule;

	if (cmdType == CONTENT_FILE) {
		FileItem *content = (FileItem *) refRule->getContent(CONTENT_FILE, cmdIndex);
		if (content != nullptr) {
            sprintf(parsed, "%s%s/%s/%s", parsed, refRule->getRootPath(), refRule->getJobDir(), content->getFileName());
		}

	} else if (cmdType == CONTENT_PARAM) {
		ParameterItem *content = (ParameterItem *) refRule->getContent(CONTENT_PARAM, cmdIndex);
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


