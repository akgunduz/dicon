//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ParameterContent_H_
#define __ParameterContent_H_

#include "Content.h"

class ParameterContent : public Content {
	PARAM_TYPES mType;
	Params mParams;
public:
	ParameterContent(){};
	ParameterContent(const char *param);
	~ParameterContent(){};
	PARAM_TYPES getParamType();
	Params getParam();
	void setParam(union Params *);
	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
