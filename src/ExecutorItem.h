//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ExecutorContent_H_
#define __ExecutorContent_H_

#include "ContentItem.h"
#include "FileItem.h"
#include "FileInfo.h"

enum EXEC_OPTIONS {
	EXEC_FILE,
	EXEC_PARAM,
	EXEC_OUTPUT,
	EXEC_MAX
};

class ExecutorItem : public ContentItem {

	char exec[PATH_MAX];
	char parsedExec[PATH_MAX];

	TypeFileInfoList fileList;

	bool parseCommand(void*, int, int);

public:
	ExecutorItem();
	ExecutorItem(const char *exec);
	~ExecutorItem(){};
    const char* getExec();
    bool parse(void *);
    const char* getParsedExec();

	TypeFileInfoList* getFileList();

	virtual CONTENT_TYPES getType();

    virtual bool isValid();
    virtual bool validate();
};



#endif //__Content_H_
