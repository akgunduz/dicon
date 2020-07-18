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

    strcpy(process, copy.getProcess());
    strcpy(parsedProcess, copy.getParsedProcess());
    fileList = copy.fileList;
};

ProcessItem::ProcessItem(const char *line)
        : ContentItem () {

	strcpy(process, line);
    strcpy(parsedProcess, "");
}

CONTENT_TYPES ProcessItem::getType() const {
	return CONTENT_PROCESS;
}

const char* ProcessItem::getProcess() const {
	return process;
}

const char *ProcessItem::getParsedProcess() const {
    return parsedProcess;
}

bool ProcessItem::parse(void *job) {

	bool cmdMode = false;
	int cmdIndex = 0;
	PROCESS_OPTIONS cmdType = PROCESS_MAX;

	for (uint32_t i = 0; i < strlen(process); i++) {
		switch(process[i]) {
			case '$':
				if (!cmdMode) {
					cmdMode = true;
					cmdIndex = 0;
					cmdType = PROCESS_MAX;
					break;
				}
				cmdMode = false;
				//no break
			case 'F':
			case 'f':
				if (cmdMode) {
					cmdType = PROCESS_FILE;
					break;
				}
				//no break
            case 'O':
            case 'o':
                if (cmdMode) {
                    cmdType = PROCESS_OUTPUT;
                    break;
                }
                //no break
			case 'P':
			case 'p':
				if (cmdMode) {
					cmdType = PROCESS_PARAM;
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
					cmdIndex = cmdIndex * 10 + (process[i] - '0');
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
                sprintf(parsedProcess, "%s%c", parsedProcess, process[i]);
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

        case PROCESS_FILE: {
            auto *content = ((JobItem*)jobItem)->getFile(cmdIndex);
            if (content != nullptr) {
                sprintf(parsedProcess, "%s%s/%s", parsedProcess, ROOT_SIGN,
                        Util::getRefPath(content->getHost().getRootPath(), content->getJobName(), content->getFileName()).c_str());
                fileList.push_back(FileInfo(content, false));
            }
        } break;

        case PROCESS_OUTPUT: {
            auto *content = (FileItem *) ((JobItem*)jobItem)->getContent(CONTENT_FILE, cmdIndex);
            if (content != nullptr) {
                sprintf(parsedProcess, "%s%s/%s", parsedProcess, ROOT_SIGN,
                        Util::getRefPath(content->getHost().getRootPath(), content->getJobName(), content->getFileName()).c_str());
                fileList.push_back(FileInfo(content, true));
            }
        } break;

        case PROCESS_PARAM: {
            auto *content = (ParameterItem *) ((JobItem*)jobItem)->getContent(CONTENT_PARAM, cmdIndex);
            if (content != nullptr) {
                strcat(parsedProcess, content->getParam());
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

void ProcessItem::setProcess(const char *_process) {

    strcpy(this->process, _process);
}

void ProcessItem::setParsedProcess(const char *_parsedProcess) {

    strcpy(this->parsedProcess, _parsedProcess);
}

void ProcessItem::addFileList(const TypeFileInfoList &_fileList) {

    fileList.insert(fileList.end(), _fileList.begin(), _fileList.end());
}

void ProcessItem::setFileList(TypeFileInfoList &_fileList) {

    fileList.clear();
    addFileList(_fileList);
}

