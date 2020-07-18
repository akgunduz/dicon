//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PROCESSITEM_H
#define DICON_PROCESSITEM_H

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
	ProcessItem(ProcessItem &);
	explicit ProcessItem(const char *);
	~ProcessItem() override = default;


    bool parse(void *);

    const char* getProcess() const;
    void setProcess(const char*);

    const char* getParsedProcess() const;
    void setParsedProcess(const char*);

    const TypeFileInfoList& getFileList() const;
    void addFileList(const TypeFileInfoList &);
    void setFileList(TypeFileInfoList&);

	CONTENT_TYPES getType() const override;

    bool isValid() override;
};



#endif //DICON_PROCESSITEM_H
