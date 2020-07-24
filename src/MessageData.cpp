//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

int MessageData::getStreamFlag() const {

    return streamFlag;
}

void MessageData::setStreamFlag(int _streamFlag) {

    streamFlag = _streamFlag;
}

long MessageData::getFileProcess() {

    return std::get<0>(fileList);
}

void MessageData::setFileProcess(long processID) {

    std::get<0>(fileList) = processID;
}

long MessageData::getFileCount() {

    return std::get<1>(fileList).size();
}

ProcessFile& MessageData::getFile(int index) {

    return std::get<1>(fileList)[index];
}

TypeProcessFileList& MessageData::getFileList() {

    return std::get<1>(fileList);
}

void MessageData::addFile(ProcessFile& fileItem) {

    std::get<1>(fileList).emplace_back(fileItem);
}

void MessageData::addFileList(long assignedProcess, TypeProcessFileList& _fileList) {

    std::get<0>(fileList) = assignedProcess;
    std::get<1>(fileList).insert(std::get<1>(fileList).end(), _fileList.begin(), _fileList.end());
}

long MessageData::getProcessCount() {

    return processList.size();;
}

ProcessItem* MessageData::getProcess(int index) {

    return processList[index];
}

void MessageData::addProcess(ProcessItem *processItem) {

    processList.emplace_back(processItem);
}

char *MessageData::getJobName() {

    return jobName;
}

void MessageData::setJobName(const char *_jobName) {

    strcpy(this->jobName, _jobName);
}

std::vector<ComponentObject> &MessageData::getComponentList() {

    return componentList;
}

void MessageData::setComponentList(std::vector<ComponentObject> &list) {

    componentList = list;
}


