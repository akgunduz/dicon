//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ExecutorItem.h"
#include "ParameterItem.h"
#include "Job.h"

ExecutorItem::ExecutorItem()
        : ContentItem () {

    strcpy(exec, "");
    strcpy(parsedExec, "");
};

ExecutorItem::ExecutorItem(const char *line)
        : ContentItem () {

	strcpy(exec, line);
    strcpy(parsedExec, "");
}

CONTENT_TYPES ExecutorItem::getType() {
	return CONTENT_EXECUTOR;
}

const char* ExecutorItem::getExec() {
	return exec;
}

const char *ExecutorItem::getParsedExec() {
    return parsedExec;
}

bool ExecutorItem::parse(void *job) {

	bool cmdMode = false;
	int cmdIndex = 0;
	CONTENT_TYPES cmdType = CONTENT_FILE;

	for (uint32_t i = 0; i < strlen(exec); i++) {
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
					parseCommand(job, cmdType, cmdIndex);
				}
				//no break
			default:
                sprintf(parsedExec, "%s%c", parsedExec, exec[i]);
				break;

		}

	}

	if (cmdMode) {
		parseCommand(job, cmdType, cmdIndex);
	}

	return true;
}

bool ExecutorItem::parseCommand(void *job, int cmdType, int cmdIndex) {

	if (cmdType == CONTENT_FILE) {
		FileItem *content = (FileItem *) ((Job*)job)->getContent(CONTENT_FILE, cmdIndex);
		if (content != nullptr) {
            sprintf(parsedExec, "%s%s", parsedExec,
                    Util::getRefPath(content->getHost(), content->getJobDir(), content->getFileName()).c_str());
            fileList.push_back(content);
		}

	} else if (cmdType == CONTENT_PARAM) {
		ParameterItem *content = (ParameterItem *) ((Job*)job)->getContent(CONTENT_PARAM, cmdIndex);
		if (content != nullptr) {
			strcat(parsedExec, content->getParam());
		}
	}

	return true;
}

bool ExecutorItem::isValid() {
    return true;
}

FileItem *ExecutorItem::getDependentFile(int index) {

    return fileList[index];
}

unsigned long ExecutorItem::getDependentFileCount() {

    return fileList.size();
}

TypeFileList *ExecutorItem::getDependentFileList() {

	return &fileList;
}
