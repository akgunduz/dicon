//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

MessageData::MessageData() :
        streamFlag(FLAG_NONE),
        processID(0),
        processCommand(""),
        jobDir("") {

}

MessageData::MessageData(MessageData &copy) {

    setProcess(copy.getProcessID(), copy.getProcessCommand());
    setJob(copy.getJobID(), copy.getJobDir());
    addMD5List(copy.getMD5List());
    addFileList(copy.getFileList(), false);

    setStreamFlag(copy.getStreamFlag());
}

unsigned long MessageData::getStreamFlag() {

    return streamFlag;
}

MessageData& MessageData::setStreamMask(unsigned long mask) {

    this->streamFlag |= mask;
    return *this;
}

MessageData& MessageData::setStreamFlag(unsigned long flag) {

    this->streamFlag = flag;
    return *this;
}

Uuid MessageData::getJobID() {

    return jobID;
}

char *MessageData::getJobDir() {

    return jobDir;
}

MessageData& MessageData::setJobID(Uuid jobID) {

    setStreamMask(FLAG_JOBID);

    this->jobID.set(&jobID);

    return *this;
}

MessageData& MessageData::setJob(Uuid jobID, const char *jobDir) {

    setStreamMask(FLAG_JOB);

    strcpy(this->jobDir, jobDir);
    this->jobID.set(&jobID);

    return *this;
}

long MessageData::getProcessID() {

    return processID;
}

char* MessageData::getProcessCommand() {

    return processCommand;
}

MessageData& MessageData::setProcessID(long id) {

    setStreamMask(FLAG_PROCESSID);

    this->processID = id;

    return *this;
}

MessageData& MessageData::setProcess(long id, const char *executor) {

    setStreamMask(FLAG_PROCESS);

    this->processID = id;
    strcpy(this->processCommand, executor);

    return *this;
}

Md5* MessageData::getMD5(int index) {

    return &md5List[index];
}

TypeMD5List *MessageData::getMD5List() {

    return &md5List;
}

unsigned long MessageData::getMD5Count() {

    return md5List.size();
}

MessageData& MessageData::addMD5(Md5 md5) {

    setStreamMask(FLAG_FILEMD5);

    md5List.push_back(md5);

    return *this;
}

MessageData& MessageData::addMD5List(TypeMD5List *list) {

    setStreamMask(FLAG_FILEMD5);

    md5List.insert(md5List.end(), list->begin(), list->end());

    return *this;
}

bool MessageData::isOutput(int index) {

    return fileList[index].isOutput();
}

FileItem* MessageData::getFile(int index) {

    return fileList[index].get();
}

TypeFileInfoList *MessageData::getFileList() {

    return &fileList;
}

unsigned long MessageData::getFileCount() {

    return fileList.size();
}

MessageData& MessageData::addFile(FileInfo file, bool isBinary) {

    setStreamMask(isBinary ? FLAG_FILEBINARY : FLAG_FILEINFO);
    fileList.push_back(file);
    return *this;
}

MessageData& MessageData::addFileList(TypeFileInfoList *list, bool isBinary) {

    setStreamMask(isBinary ? FLAG_FILEBINARY : FLAG_FILEINFO);
    fileList.insert(fileList.end(), list->begin(), list->end());
    return *this;
}

MessageData &MessageData::operator=(MessageData *other) {

    if (this != other) {
        setProcess(other->getProcessID(), other->getProcessCommand());
        setJob(other->getJobID(), other->getJobDir());
        addMD5List(other->getMD5List());
        addFileList(other->getFileList(), false);

        setStreamFlag(other->getStreamFlag());
    }

    return *this;
}
