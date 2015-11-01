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

	Md5 md5;
	bool flaggedToSent;
	FILETYPE fileType;

    void setRootPath(const char* rootPath);

public:
	FileItem(const char* rootPath);
    FileItem(FileItem*);
	FileItem(const char*, const char *,
             const char*, FILETYPE,
             Md5 *md5 = nullptr);
	~FileItem(){};
    bool set(const char *, const char *,
             const char *, FILETYPE, Md5*);
    void set(FileItem*);
	bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	FILETYPE getFileType();
	Md5* getMD5();
	void setMD5(Md5* = nullptr);
    const char* getFileName();
    const char* getRootPath();
    const char* getJobDir();

    void setFile(const char* , const char*,
                 const char*, FILETYPE);
    std::string getAbsPath();
    std::string getAbsPath(Unit, Unit);
    std::string getMD5Path();

	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
