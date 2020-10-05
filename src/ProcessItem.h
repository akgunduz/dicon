//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PROCESSITEM_H
#define DICON_PROCESSITEM_H

#include "ProcessState.h"
#include "ProcessFile.h"

enum PROCESS_OPTIONS {
	PROCESS_INPUT,
	PROCESS_PARAM,
	PROCESS_OUTPUT,
	PROCESS_MAX
};

class ProcessItem : public ContentItem {

    PROCESS_STATE state{PROCESS_STATE_DEPENDENT};

    std::string process;
    std::string parsedProcess;

    TypeProcessFileList fileList;

    long assignedComponent{};

    bool parseCommand(void*, int, int);

public:

	ProcessItem(const ProcessItem &) = default;
	explicit ProcessItem(const TypeHostUnit&, long = 0, long = 0, std::string = "");
	~ProcessItem() override = default;

    bool parse(void *);

    const std::string& getProcess();
    void setProcess(const std::string&);

    const std::string& getParsedProcess();
    void setParsedProcess(const std::string&);

    size_t getFileCount();
    TypeProcessFile getFile(const TypeProcessFile&);
    const TypeProcessFileList& getFileList();
    void addFile(const TypeFileItem&, long, bool);
    void addFile(const TypeProcessFile&);
    void addFileList(const TypeProcessFileList&);

    PROCESS_STATE getState();
    void setState(PROCESS_STATE);

    long getAssigned();
    void setAssigned(long);

    void setID(TypeID) override;

	CONTENT_TYPES getType() override;

    bool check() override;
};

typedef std::shared_ptr<ProcessItem> TypeProcessItem;
typedef std::vector<TypeProcessItem> TypeProcessList;

#endif //DICON_PROCESSITEM_H
