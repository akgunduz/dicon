//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ProcessItem.h"
#include "ParameterItem.h"
#include "JobItem.h"
#include "Util.h"

ProcessItem::ProcessItem(const ProcessItem &copy) :
        ContentItem(copy),
        assignedComponent(copy.assignedComponent),
        state(copy.state),
        duration(copy.duration) {

    strcpy(process, copy.getProcess());
    strcpy(parsedProcess, copy.getParsedProcess());
    fileList = copy.fileList;
};

ProcessItem::ProcessItem(const ComponentObject& host, long id, long jobID, const char *_process) :
	ContentItem(host, id, jobID) {

	if (_process) {
		strcpy(process, _process);
	}
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
            case 'I':
            case 'i':
				if (cmdMode) {
					cmdType = PROCESS_INPUT;
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

        case PROCESS_INPUT:
		case PROCESS_OUTPUT: {
            auto *content = ((JobItem*)jobItem)->getFile(cmdIndex - 1);
            if (content != nullptr) {
                sprintf(parsedProcess, "%s%s/%s", parsedProcess, ROOT_SIGN,
                        Util::getRefPath(content->getHost().getRootPath(), content->getAssignedJob(), content->getName()).c_str());
				addFile(content, getID(), cmdType == PROCESS_OUTPUT);
            }
        } break;

        case PROCESS_PARAM: {
            auto *content = ((JobItem*)jobItem)->getParameter(cmdIndex - 1);
            if (content != nullptr) {
                strcat(parsedProcess, content->getParam());
            }
        } break;

        default:
            break;
    }

	return true;
}

bool ProcessItem::check() {

	for (auto & file : fileList) {

	    if (file.isOutput()) {
	        continue;
	    }

	    if (!file.get()->check()) {
	        return false;
	    }
	}

    return true;
}

CONTENT_TYPES ProcessItem::getType() const {
	return CONTENT_PROCESS;
}

const char* ProcessItem::getProcess() const {
	return process;
}

void ProcessItem::setProcess(const char *_process) {

	strcpy(this->process, _process);
}

const char *ProcessItem::getParsedProcess() const {
	return parsedProcess;
}

void ProcessItem::setParsedProcess(const char *_parsedProcess) {

	strcpy(this->parsedProcess, _parsedProcess);
}

int ProcessItem::getFileCount() const {

    return fileList.size();
}

ProcessFile& ProcessItem::getFile(ProcessFile& ref) {

    for (auto& file : fileList) {
        if (file.get()->getID() == ref.get()->getID()) {
            return file;
        }
    }
    return *fileList.end();
}

TypeProcessFileList& ProcessItem::getFileList() {

	return fileList;
}

void ProcessItem::addFile(FileItem *file, long processID, bool isOutput) {

    fileList.emplace_back(ProcessFile(file, processID, isOutput));
}

void ProcessItem::addFileList(TypeProcessFileList& _fileList) {

    fileList.insert(fileList.end(), _fileList.begin(), _fileList.end());
}

PROCESS_STATE ProcessItem::getState() const {

	return state;
}

void ProcessItem::setState(PROCESS_STATE _state) {

	state = _state;
}

long ProcessItem::getAssigned() const {

	return assignedComponent;
}

void ProcessItem::setAssigned(long _assignedComponent) {

	assignedComponent = _assignedComponent;
}

void ProcessItem::setID(long _id) {

    ContentItem::setID(_id);

    for (auto& file : getFileList()) {
        file.setAssignedProcess(_id);
    }
}

long ProcessItem::getDuration() const {
    return duration;
}

void ProcessItem::setDuration(long _duration) {
    duration = _duration;
}
