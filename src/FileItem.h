//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "ContentItem.h"
#include "Unit.h"
#include "Md5.h"

#define MAX_FILE_NAME_LENGTH 100
#define MAX_JOB_DIR_LENGTH 100

class FileItem : public ContentItem {

	char fileName[MAX_FILE_NAME_LENGTH];
	char jobDir[MAX_JOB_DIR_LENGTH];
	char rootPath[PATH_MAX];
	char absPath[PATH_MAX];
	char md5Path[PATH_MAX];

	Md5 md5;
	bool flaggedToSent;
	FILETYPE fileType;

    Unit host;

    void setRootPath(const char* rootPath);

public:
	FileItem(Unit host, const char* rootPath);
    FileItem(FileItem*);
    FileItem(Unit host, const char*, const char *,
             const char*, FILETYPE,
             Md5 *md5 = nullptr);
	FileItem(Unit host, Unit node, const char*, const char *,
             const char*, FILETYPE,
             Md5 *md5 = nullptr);
	~FileItem(){};

    bool set(Unit host, Unit node, const char *, const char *,
             const char *, FILETYPE, Md5* = nullptr);

    bool set(Unit host, const char *, const char *,
             const char *, FILETYPE, Md5* = nullptr);

    bool set(FileItem*);
	bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	FILETYPE getFileType();
	Md5* getMD5();
	void setMD5(Md5* = nullptr);
    const char* getFileName();
    const char* getRootPath();
    const char* getJobDir();
    Unit getHost();

    void setFile(Unit node, const char* , const char*,
                 const char*, FILETYPE);
    const char* getAbsPath();
    const char* getRefPath();
    const char* getMD5Path();

	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

#endif //__Content_H_
