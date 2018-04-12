//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

int MessageData::getStreamFlag() {

    return streamFlag;
}

void MessageData::setStreamFlag(int streamFlag) {

    this->streamFlag = streamFlag;
}

//char *MessageData::getJobDir() {
//
//    return jobDir;
//}

//void MessageData::setJob(int streamFlag) {
//
//    this->streamFlag = streamFlag;
//}


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

FileItem *MessageData::getFile(int index) {

    return fileList[index];
}

TypeFileList *MessageData::getFileList() {

    return &fileList;
}

void MessageData::addFile(FileItem* file) {

    fileList.push_back(file);
}

void MessageData::addFileList(TypeFileList *list) {

    fileList.insert(fileList.end(), list->begin(), list->end());
}

unsigned long MessageData::getFileCount() {

    return fileList.size();
}

char* MessageData::getExecutor() {

    return executor;
}

void MessageData::setExecutor(char *executor) {

    strcpy(this->executor, executor);
}

char *MessageData::getJobDir() {

    return jobDir;
}

void MessageData::setJobDir(char *jobDir) {

    strcpy(this->jobDir, jobDir);
}
