//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "ContentItem.h"
#include "Md5.h"

class FileItem : public ContentItem {

	ComponentObject host;
	char fileName[PATH_MAX];
	char jobDir[PATH_MAX];

	int id;
	bool is_exist;

	Md5 md5;

public:
	FileItem(ComponentObject);
    FileItem(FileItem*);
    FileItem(ComponentObject host, const char*, const char*,
             int id, Md5 *md5 = nullptr);
	~FileItem(){};

    void set(const char*, const char*,
             int id, Md5* = NULL);

	Md5* getMD5();
	void setMD5(Md5* = nullptr);
    const char* getFileName();
    const char* getJobDir();

	ComponentObject getHost();
    int getID();

    bool validate();

	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

typedef std::vector<FileItem*> TypeFileList;

#endif //__Content_H_
