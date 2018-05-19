//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_PROCESSITEM_H
#define BANKOR_PROCESSITEM_H

#include "ContentItem.h"
#include "FileItem.h"
#include "FileInfo.h"

enum PROCESS_OPTIONS {
	PROCESS_FILE,
	PROCESS_PARAM,
	PROCESS_OUTPUT,
	PROCESS_MAX
};

class ProcessItem : public ContentItem {

	char process[PATH_MAX];
	char parsedProcess[PATH_MAX];

	TypeFileInfoList fileList;

	bool parseCommand(void*, int, int);

public:
	ProcessItem();
	ProcessItem(const char *);
	~ProcessItem(){};
    const char* getProcess();
    bool parse(void *);
    const char* getParsedProcess();

	TypeFileInfoList* getFileList();

	virtual CONTENT_TYPES getType();

    virtual bool isValid();
    virtual bool validate();
};



#endif //BANKOR_PROCESSITEM_H
