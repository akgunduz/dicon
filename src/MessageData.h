//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEDATA_H
#define DICON_MESSAGEDATA_H

#include "ProcessItem.h"
#include "ComponentUnit.h"

class MessageData {

    std::string jobName;

    uint32_t processCount;

    TypeID id;

    TypeProcessItem process;

    TypeFileItem file;

    TypeProcessFile processFile;

    TypeComponentUnitList componentList;

public:

    explicit MessageData(const TypeHostUnit&);
    ~MessageData() = default;

    TypeID& getID();
    void setID(TypeID&);

    const TypeProcessItem& getProcess();
    void setProcess(const TypeProcessItem&);
    void setProcess(TypeID, const TypeProcessFileList&);
    void setProcess(TypeID, const TypeProcessFile&);

    const TypeFileItem& getFile();
    void setFile(const TypeFileItem&);

    const TypeProcessFile& getProcessFile();
    void setProcessFile(const TypeProcessFile&);

    std::string& getJobName();
    void setJobName(const std::string&);

    TypeComponentUnitList& getComponentList();
    void setComponentList(TypeComponentUnitList&);

    uint32_t& getProcessCount();
    void setProcessCount(uint32_t&);
};

#endif //DICON_MESSAGEDATA_H
