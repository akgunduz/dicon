//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ParameterContent_H_
#define __ParameterContent_H_

#include "ContentItem.h"

class ParameterItem : public ContentItem {
	char sPtr[NAME_MAX];
public:
	ParameterItem(const ComponentObject&, const char *);
	~ParameterItem(){};
	const char* getParam();
	void setParam(const char*);
	virtual CONTENT_TYPES getType() const;

    virtual bool check();
};

#endif //__Content_H_
