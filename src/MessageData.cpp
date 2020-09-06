//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

MessageData::MessageData(const TypeHostUnit& host) {

    process = std::make_shared<ProcessItem>(host);

    file = std::make_shared<FileItem>(host, true);

    processFile = std::make_shared<ProcessFile>(file, 0, false);
}

const TypeFileItem& MessageData::getFile() {

    return file;
}

void MessageData::setFile(const TypeFileItem& _file) {

    file = _file;
}

const TypeProcessFile& MessageData::getProcessFile() {

    return processFile;
}

void MessageData::setProcessFile(const TypeProcessFile& _processFile) {

    processFile = _processFile;
}

const TypeProcessItem& MessageData::getProcess() {

    return process;
}

void MessageData::setProcess(const TypeProcessItem& _process) {

    process = _process;
}

void MessageData::setProcess(TypeID _id, const TypeProcessFileList& _fileList) {

    process->addFileList(_fileList);
    process->setID(_id);
}

void MessageData::setProcess(TypeID _id, const TypeProcessFile& _file) {

    process->addFile(_file);
    process->setID(_id);
}

std::string& MessageData::getJobName() {

    return jobName;
}

void MessageData::setJobName(const std::string& _jobName) {

    jobName = _jobName;
}

TypeComponentUnitList& MessageData::getComponentList() {

    return componentList;
}

void MessageData::setComponentList(TypeComponentUnitList& list) {

    componentList = list;
}

TypeID& MessageData::getID() {

    return id;
}

void MessageData::setID(TypeID& _id) {

    id = _id;
}

uint32_t& MessageData::getProcessCount() {

    return processCount;
}

void MessageData::setProcessCount(uint32_t& _count) {

    processCount = _count;
}
