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

class ProcessItem : public ContentItem {

	char exec[PATH_MAX];
	char parsedExec[PATH_MAX];

	TypeFileInfoList fileList;

	bool parseCommand(void*, int, int);

public:
	ProcessItem();
	ProcessItem(ProcessItem &);
	explicit ProcessItem(const char *exec);
	~ProcessItem() override = default;


    bool parse(void *);

    const char* getExec() const;
    void setExec(const char*);

    const char* getParsedExec() const;
    void setParsedExec(const char*);

    const TypeFileInfoList& getFileList() const;
    void addFileList(const TypeFileInfoList &);
    void setFileList(TypeFileInfoList&);



	CONTENT_TYPES getType() const override;

    bool isValid() override;
};



#endif //__Content_H_
