//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEDATA_H
#define DICON_MESSAGEDATA_H

#include "ProcessItem.h"

class MessageData {

    int streamFlag{};

    std::string jobName;

    std::pair<long, TypeProcessFileList> fileList;

    TypeProcessList processList;

    TypeComponentUnitList componentList;

public:

    MessageData() = default;
    ~MessageData();

    int getStreamFlag() const;
    void setStreamFlag(int);

    ProcessFile& getFile(int);
    long getFileProcess();
    void setFileProcess(long);
    TypeProcessFileList& getFileList();
    void addFile(ProcessFile&);
    void addFileList(long, TypeProcessFileList&);
    long getFileCount();

    TypeProcessItem& getProcess(int);
    void addProcess(const TypeProcessItem&);
    long getProcessCount();

    const std::string& getJobName() const;
    void setJobName(const std::string&);

    TypeComponentUnitList& getComponentList();
    void setComponentList(TypeComponentUnitList&);
};

#endif //DICON_MESSAGEDATA_H
