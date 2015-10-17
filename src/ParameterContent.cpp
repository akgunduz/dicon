//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ParameterContent.h"
#include "Rule.h"

ParameterContent::ParameterContent(const char *param) {

	setValid(true);

	if (strncmp(param, "l:", 2) == 0) {
		mType = PARAM_LONG;
		mParams.latom = atol(param + 2);
	} else if (strncmp(param, "d:", 2) == 0) {
		mType = PARAM_DOUBLE;
		mParams.datom = atof(param + 2);
	} else {
		mType = PARAM_STRING;
		strcpy(mParams.sPtr, param + 2);
	}
};

CONTENT_TYPES ParameterContent::getType() {
	return CONTENT_PARAM;
}

PARAM_TYPES ParameterContent::getParamType() {
	return mType;
}

Params ParameterContent::getParam() {
	return mParams;
}

void ParameterContent::setParam(Params *param) {
	memcpy(&mParams, param, sizeof(Params));
}