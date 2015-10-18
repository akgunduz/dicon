//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "Content.h"
#include "Unit.h"

class FileContent : public Content {
	char path[PATH_MAX];
	char absPath[PATH_MAX];
	char md5Path[PATH_MAX];
	uint8_t md5[MD5_DIGEST_LENGTH];
	bool flaggedToSent;
	FILETYPE fileType;

public:
	FileContent();
	FileContent(Unit host, Unit node, const char* rootPath,
                const char *path, FILETYPE fileType);
	~FileContent(){};
	bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	FILETYPE getFileType();
	uint8_t* getMD5();
	void setMD5(uint8_t*);
    const char* getPath();
    void setFile(Unit host, Unit node, const char* rootPath,
                 const char *path, FILETYPE fileType);
    const char* getAbsPath();
	const char* getMD5Path();

	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
