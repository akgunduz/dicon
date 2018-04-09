//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ParameterItem.h"

ParameterItem::ParameterItem()
    : ContentItem () {

};

ParameterItem::ParameterItem(const char *param)
        : ContentItem () {

	strcpy(sPtr, param);
};

CONTENT_TYPES ParameterItem::getType() {
	return CONTENT_PARAM;
}

void ParameterItem::setParam(const char *param) {
	strcpy(sPtr, param);
}

bool ParameterItem::isValid() {
    return true;
}

const char *ParameterItem::getParam() {
	return sPtr;
}
