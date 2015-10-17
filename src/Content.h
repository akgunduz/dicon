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

#endif //__Content_H_
