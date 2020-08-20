//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

MessageData::~MessageData() = default;

int MessageData::getStreamFlag() const {

    return streamFlag;
}

void MessageData::setStreamFlag(int _streamFlag) {

    streamFlag = _streamFlag;
}

long MessageData::getFileProcess() {

    return fileList.first;
}

void MessageData::setFileProcess(long processID) {

    fileList.first = processID;
}

long MessageData::getFileCount() {

    return fileList.second.size();
}

ProcessFile& MessageData::getFile(int index) {

    return fileList.second[index];
}

TypeProcessFileList& MessageData::getFileList() {

    return fileList.second;
}

void MessageData::addFile(ProcessFile& fileItem) {

    fileList.second.emplace_back(fileItem);
}

void MessageData::addFileList(long assignedProcess, TypeProcessFileList& _fileList) {

    fileList.first = assignedProcess;
    fileList.second.insert(fileList.second.end(), _fileList.begin(), _fileList.end());
}

long MessageData::getProcessCount() {

    return processList.size();
}

TypeProcessItem MessageData::getProcess(int index) {

    return processList[index];
}

void MessageData::addProcess(const TypeProcessItem& processItem) {

    processList.emplace_back(processItem);
}

char *MessageData::getJobName() {

    return jobName;
}

void MessageData::setJobName(const char *_jobName) {

    strcpy(this->jobName, _jobName);
}

TypeComponentList &MessageData::getComponentList() {

    return componentList;
}

void MessageData::setComponentList(TypeComponentList &list) {

    componentList = list;
}
