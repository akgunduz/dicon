//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ParameterItem.h"

ParameterItem::ParameterItem(const TypeHostUnit& host, long id, long jobID, const char *param)
        : ContentItem (host, id, jobID) {

	strcpy(sPtr, param);
};

CONTENT_TYPES ParameterItem::getType() const {
	return CONTENT_PARAM;
}

void ParameterItem::setParam(const char *param) {
	strcpy(sPtr, param);
}

bool ParameterItem::check() {
    return true;
}

const char *ParameterItem::getParam() {
	return sPtr;
}
