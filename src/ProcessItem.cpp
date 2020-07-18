//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ProcessItem.h"
#include "ParameterItem.h"
#include "Job.h"

ProcessItem::ProcessItem()
        : ProcessItem("") {
};

ProcessItem::ProcessItem(ProcessItem &copy) {

    strcpy(exec, copy.getExec());
    strcpy(parsedExec, copy.getParsedExec());
    fileList = copy.fileList;
};

ProcessItem::ProcessItem(const char *line)
        : ContentItem () {

	strcpy(exec, line);
    strcpy(parsedExec, "");
}

CONTENT_TYPES ProcessItem::getType() const {
	return CONTENT_EXECUTOR;
}

const char* ProcessItem::getExec() const {
	return exec;
}

const char *ProcessItem::getParsedExec() const {
    return parsedExec;
}

bool ProcessItem::parse(void *job) {

	bool cmdMode = false;
	int cmdIndex = 0;
	EXEC_OPTIONS cmdType = EXEC_MAX;

	for (uint32_t i = 0; i < strlen(exec); i++) {
		switch(exec[i]) {
			case '$':
				if (!cmdMode) {
					cmdMode = true;
					cmdIndex = 0;
					cmdType = EXEC_MAX;
					break;
				}
				cmdMode = false;
				//no break
			case 'F':
			case 'f':
				if (cmdMode) {
					cmdType = EXEC_FILE;
					break;
				}
				//no break
            case 'O':
            case 'o':
                if (cmdMode) {
                    cmdType = EXEC_OUTPUT;
                    break;
                }
                //no break
			case 'P':
			case 'p':
				if (cmdMode) {
					cmdType = EXEC_PARAM;
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

bool ProcessItem::parseCommand(void *jobItem, int cmdType, int cmdIndex) {

    switch (cmdType) {

        case EXEC_FILE: {
            auto *content = ((JobItem*)jobItem)->getFile(cmdIndex);
            if (content != nullptr) {
                sprintf(parsedExec, "%s%s/%s", parsedExec, ROOT_SIGN,
                        Util::getRefPath(content->getHost().getRootPath(), content->getJobName(), content->getFileName()).c_str());
                fileList.push_back(FileInfo(content, false));
            }
        } break;

        case EXEC_OUTPUT: {
            auto *content = (FileItem *) ((JobItem*)jobItem)->getContent(CONTENT_FILE, cmdIndex);
            if (content != nullptr) {
                sprintf(parsedExec, "%s%s/%s", parsedExec, ROOT_SIGN,
                        Util::getRefPath(content->getHost().getRootPath(), content->getJobName(), content->getFileName()).c_str());
                fileList.push_back(FileInfo(content, true));
            }
        } break;

        case EXEC_PARAM: {
            auto *content = (ParameterItem *) ((JobItem*)jobItem)->getContent(CONTENT_PARAM, cmdIndex);
            if (content != nullptr) {
                strcat(parsedExec, content->getParam());
            }
        } break;

        default:
            break;
    }

	return true;
}

bool ProcessItem::isValid() {

	for (auto & file : fileList) {

	    if (file.isOutput()) {
	        continue;
	    }

	    if (!file.get()->isValid()) {
	        return false;
	    }
	}

    return true;
}

const TypeFileInfoList& ProcessItem::getFileList() const {

    return fileList;
}

void ProcessItem::setExec(const char *_exec) {

    strcpy(this->exec, _exec);
}

void ProcessItem::setParsedExec(const char *_parsedExec) {

    strcpy(this->parsedExec, _parsedExec);
}

void ProcessItem::addFileList(const TypeFileInfoList &_fileList) {

    fileList.insert(fileList.end(), _fileList.begin(), _fileList.end());
}

void ProcessItem::setFileList(TypeFileInfoList &_fileList) {

    fileList.clear();
    addFileList(_fileList);
}

