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
	const char* jobDir;

	Md5 md5;
	bool flaggedToSent;
	FILETYPE fileType;

    Unit host;
    Unit node;

	std::string getPath(bool);

public:
	FileItem(Unit host);
    FileItem(const char *, FileItem*);
    FileItem(Unit host, const char *, const char*, FILETYPE,
             Md5 *md5 = nullptr);
	FileItem(Unit host, Unit node, const char *, const char*, FILETYPE,
             Md5 *md5 = nullptr);
	~FileItem(){};

    bool set(Unit host, Unit node, const char *, const char *, FILETYPE, Md5* = nullptr);

    bool set(Unit host, const char *, const char *, FILETYPE, Md5* = nullptr);

    bool set(const char*, FileItem*);
	bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	FILETYPE getFileType();
	Md5* getMD5();
	void setMD5(Md5* = nullptr);
    const char* getFileName();
    const char* getJobDir();

    Unit getHost();

    void setFile(Unit, const char*, const char*, FILETYPE);
	std::string getRefPath();
	std::string getMD5Path();


	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

typedef std::vector<FileItem*> TypeFileList;

#endif //__Content_H_
