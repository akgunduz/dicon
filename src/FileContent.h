//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "Content.h"

class FileContent : public Content {
	char path[PATH_MAX];
	char absPath[PATH_MAX];
	uint8_t md5[MD5_DIGEST_LENGTH];
	bool flaggedToSent;
	FILETYPE fileType;

public:
	FileContent();
	FileContent(const char*, const char*, const char*, FILETYPE);
	~FileContent(){};
	bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	FILETYPE getFileType();
	void setFileType(FILETYPE);
	uint8_t* getMD5();
	void setMD5(uint8_t*);
    const char* getPath();
    const char* getAbsPath();
	void setPath(const char*);
	void setAbsPath(const char *path);
	void set(FileContent *);
	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
