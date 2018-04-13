//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "ContentItem.h"
#include "Md5.h"

#define MAX_FILE_NAME_LENGTH 100
#define MAX_JOB_DIR_LENGTH 100

class FileItem : public ContentItem {

    COMPONENT host;
	char fileName[MAX_FILE_NAME_LENGTH];
	char jobDir[MAX_JOB_DIR_LENGTH];

	bool info_only;

	Md5 md5;


public:
	FileItem(COMPONENT host);
    FileItem(FileItem*);
    FileItem(COMPONENT host, const char *, const char*,
             Md5 *md5 = nullptr, bool = false);
	~FileItem(){};

    bool set(COMPONENT host, const char *, const char *, Md5* = nullptr, bool = false);
   // bool set(const char*, FileItem*);

	Md5* getMD5();
	void setMD5(Md5* = nullptr);
    const char* getFileName();
    const char* getJobDir();

    COMPONENT getHost();

   // void setFile(const char*, const char*, Md5* = nullptr);



	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

typedef std::vector<FileItem*> TypeFileList;

#endif //__Content_H_
