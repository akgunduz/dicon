//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ParameterItem.h"
#include "Rule.h"

ParameterItem::ParameterItem()
    : ContentItem () {

};

ParameterItem::ParameterItem(const char *param)
        : ContentItem () {

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

CONTENT_TYPES ParameterItem::getType() {
	return CONTENT_PARAM;
}

PARAM_TYPES ParameterItem::getParamType() {
	return mType;
}

Params ParameterItem::getParam() {
	return mParams;
}

void ParameterItem::setParam(Params *param) {
	memcpy(&mParams, param, sizeof(Params));
}

bool ParameterItem::isValid() {
    return true;
}
