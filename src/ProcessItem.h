//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PROCESSITEM_H
#define DICON_PROCESSITEM_H

#include "ContentItem.h"
#include "FileItem.h"
#include "ProcessState.h"
#include "ProcessFile.h"

typedef std::vector<ProcessFile> TypeProcessFileList;

enum PROCESS_OPTIONS {
	PROCESS_INPUT,
	PROCESS_PARAM,
	PROCESS_OUTPUT,
	PROCESS_MAX
};

class ProcessItem : public ContentItem {

    PROCESS_STATE state{PROCESS_STATE_DEPENDENT};

	char process[PATH_MAX]{};
	char parsedProcess[PATH_MAX]{};

    TypeProcessFileList fileList;

    long assignedComponent{};

	bool parseCommand(void*, int, int);

public:

	ProcessItem(const ProcessItem &);
	explicit ProcessItem(const ComponentObject&, long = 0, long = 0, const char * = nullptr);
	~ProcessItem() override = default;

    bool parse(void *);

    const char* getProcess() const;
    void setProcess(const char*);

    const char* getParsedProcess() const;
    void setParsedProcess(const char*);

    int getFileCount() const;
    ProcessFile& getFile(ProcessFile&);
    TypeProcessFileList& getFileList();
    void addFile(FileItem*, long, bool);
    void addFileList(TypeProcessFileList& fileList);

    PROCESS_STATE getState() const;
    void setState(PROCESS_STATE);

    long getAssigned() const;
    void setAssigned(long);

    void setID(long);

	CONTENT_TYPES getType() const override;

    bool check() override;
};

typedef std::vector<ProcessItem*> TypeProcessList;

#endif //DICON_PROCESSITEM_H
