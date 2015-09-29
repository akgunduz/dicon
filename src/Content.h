//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Content_H_
#define __Content_H_

#include "Common.h"

#define BUFFER_SIZE 512

enum CONTENT_TYPES {
	CONTENT_FILE,
	CONTENT_PARAM,
	CONTENT_EXECUTOR
};

enum PARAM_TYPES {
	PARAM_LONG,
	PARAM_DOUBLE,
	PARAM_STRING
};

union Params {
	long latom;
	double datom;
	char sPtr[255];
};

class Content {
	bool mIsValid;
public:
	bool isValid();
	void setValid(bool);
	Content();
	virtual CONTENT_TYPES getType() = 0;
	virtual ~Content(){};
};

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

class ParamContent : public Content {
	PARAM_TYPES mType;
	Params mParams;
public:
	ParamContent(){};
	ParamContent(const char *param);
	~ParamContent(){};
	PARAM_TYPES getParamType();
	Params getParam();
	void setParam(union Params *);
	virtual CONTENT_TYPES getType();
};

class ExecutorContent : public Content {
	std::string exec;
	bool parseCommand(std::string&, void *, int, int);
public:
	ExecutorContent(){};
	ExecutorContent(const char *exec);
	~ExecutorContent(){};
	std::string getExec();
	std::string getParsed(void *);
	void setExec(const std::string&);
	virtual CONTENT_TYPES getType();
};



#endif //__Content_H_
