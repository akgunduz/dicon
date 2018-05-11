//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

MessageData::MessageData() :
        streamFlag(STREAM_NONE),
        executorID(0),
        executor(""),
        jobDir("") {

}

MessageData::MessageData(MessageData &copy) {

    setStreamFlag(copy.getStreamFlag());
    setExecutor(copy.getExecutorID(), copy.getExecutor());
    setJob(copy.getJobID(), copy.getJobDir());
    addMD5List(copy.getMD5List());
    addFileList(copy.getFileList());
}

int MessageData::getStreamFlag() {

    return streamFlag;
}

MessageData& MessageData::setStreamFlag(int streamFlag) {

    this->streamFlag = streamFlag;
    return *this;
}

Md5* MessageData::getMD5(int index) {

    return &md5List[index];
}

TypeMD5List *MessageData::getMD5List() {

    return &md5List;
}

MessageData& MessageData::addMD5(Md5 md5) {

    md5List.push_back(md5);
    return *this;
}

MessageData& MessageData::addMD5List(TypeMD5List *list) {

    md5List.insert(md5List.end(), list->begin(), list->end());
    return *this;
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

TypeFileInfoList *MessageData::getFileList() {

    return &fileList;
}

MessageData& MessageData::addFile(FileInfo file) {

    fileList.push_back(file);
    return *this;
}

MessageData& MessageData::addFileList(TypeFileInfoList *list) {

    fileList.insert(fileList.end(), list->begin(), list->end());
    return *this;
}

unsigned long MessageData::getFileCount() {

    return fileList.size();
}

char* MessageData::getExecutor() {

    return executor;
}

long MessageData::getExecutorID() {

    return executorID;
}

MessageData& MessageData::setExecutorID(long id) {

    this->executorID = id;
    return *this;
}

MessageData& MessageData::setExecutor(long id, const char *executor) {

    this->executorID = id;
    strcpy(this->executor, executor);
    return *this;
}

Uuid MessageData::getJobID() {

    return jobID;
}

char *MessageData::getJobDir() {

    return jobDir;
}

MessageData& MessageData::setJob(Uuid jobID, const char *jobDir) {

    strcpy(this->jobDir, jobDir);
    return setJobID(jobID);
}

MessageData& MessageData::setJobID(Uuid jobID) {

    this->jobID.set(&jobID);
    return *this;
}

MessageData &MessageData::operator=(MessageData *other) {

    if (this != other) {
        setStreamFlag(other->getStreamFlag());
        setExecutor(other->getExecutorID(), other->getExecutor());
        setJob(other->getJobID(), other->getJobDir());
        addMD5List(other->getMD5List());
        addFileList(other->getFileList());
    }

    return *this;
}

