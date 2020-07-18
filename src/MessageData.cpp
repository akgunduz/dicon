//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

int MessageData::getStreamFlag() {

    return streamFlag;
}

void MessageData::setStreamFlag(int _streamFlag) {

    this->streamFlag = _streamFlag;
}

Md5* MessageData::getMD5(int index) {

    return &md5List[index];
}

TypeMD5List *MessageData::getMD5List() {

    return &md5List;
}

void MessageData::addMD5(Md5 md5) {

    md5List.push_back(md5);
}

void MessageData::addMD5List(TypeMD5List *list) {

    md5List.insert(md5List.end(), list->begin(), list->end());
}

unsigned long MessageData::getMD5Count() {

    return md5List.size();
}

FileItem* MessageData::getFile(int index) {

    return fileList[index].get();
}

bool MessageData::isOutput(int index) {

    return fileList[index].isOutput();
}

TypeFileInfoList& MessageData::getFileList() {

    return fileList;
}

void MessageData::addFile(FileInfo file) {

    fileList.push_back(file);
}

void MessageData::addFileList(const TypeFileInfoList &list) {

    fileList.insert(fileList.end(), list.begin(), list.end());
}

unsigned long MessageData::getFileCount() {

    return fileList.size();
}

char* MessageData::getProcess() {

    return process;
}

int MessageData::getProcessID() {

    return processID;
}

void MessageData::setProcessID(int _processID) {

    this->processID = _processID;
}

void MessageData::setProcess(int _processID, const char *_process) {

    this->processID = _processID;
    strcpy(this->process, _process);
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
