//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "Content.h"

class FileContent : public Content {
	std::string mPath;
	uint8_t mMD5[MD5_DIGEST_LENGTH];
	bool mFlaggedToSent;

public:
	FileContent();
	FileContent(const std::string&, const std::string&, const char *);
	~FileContent(){};
	const bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	uint8_t* getMD5();
	void setMD5(uint8_t*);
	std::string getPath();
	void setPath(const std::string&);
	void set(FileContent *);
	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
