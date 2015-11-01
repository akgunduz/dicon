//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ParameterContent_H_
#define __ParameterContent_H_

#include "ContentItem.h"

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

class ParameterItem : public ContentItem {
	PARAM_TYPES mType;
	Params mParams;
public:
	ParameterItem();
	ParameterItem(const char *param);
	~ParameterItem(){};
	PARAM_TYPES getParamType();
	Params getParam();
	void setParam(union Params *);
	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
