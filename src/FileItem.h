//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "ContentItem.h"
#include "Md5.h"

class FileItem : public ContentItem {

    COMPONENT host;
	char fileName[PATH_MAX];
	char jobDir[PATH_MAX];

	int id;
	bool is_validated;

	Md5 md5;


public:
	FileItem(COMPONENT host);
    FileItem(FileItem*);
    FileItem(COMPONENT host, const char*, const char*,
             int id, Md5 *md5 = nullptr);
	~FileItem(){};

    void set(COMPONENT host, const char*, const char*,
             int id, Md5* = NULL);

	Md5* getMD5();
	void setMD5(Md5* = nullptr);
    const char* getFileName();
    const char* getJobDir();

    COMPONENT getHost();
    int getID();

    bool validate();

	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

typedef std::vector<FileItem*> TypeFileList;

#endif //__Content_H_
