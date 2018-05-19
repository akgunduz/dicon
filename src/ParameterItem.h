//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_PARAMETERITEM_H
#define BANKOR_PARAMETERITEM_H

#include "ContentItem.h"

class ParameterItem : public ContentItem {
	char sPtr[255];
public:
	ParameterItem();
	ParameterItem(const char *param);
	~ParameterItem(){};
	const char* getParam();
	void setParam(const char*);
	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

#endif //BANKOR_PARAMETERITEM_H
