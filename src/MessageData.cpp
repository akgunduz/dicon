//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageData.h"

MessageData::MessageData(const TypeHostUnit& host) {

    process = std::make_shared<ProcessItem>(host);
}

int MessageData::getStreamType() {

    return streamType;
}

void MessageData::setStreamType(int _streamFlag) {

    streamType = _streamFlag;
}

const TypeProcessItem& MessageData::getProcess() {

    return process;
}

void MessageData::setProcess(const TypeProcessItem& _process) {

    process = _process;
}

void MessageData::setProcess(long id, const TypeProcessFileList& _fileList) {

    process->addFileList(_fileList);
    process->setID(id);
}

void MessageData::setProcess(long id, const TypeProcessFile& _file) {

    process->addFile(_file);
    process->setID(id);
}

std::string& MessageData::getJobName() {

    return jobName;
}

void MessageData::setJobName(const std::string& _jobName) {

    jobName = _jobName;
}

TypeComponentUnitList &MessageData::getComponentList() {

    return componentList;
}

void MessageData::setComponentList(TypeComponentUnitList &list) {

    componentList = list;
}
