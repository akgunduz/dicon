//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ExecutorContent_H_
#define __ExecutorContent_H_

#include "ContentItem.h"

class ExecutorItem : public ContentItem {
	char exec[PATH_MAX];
	bool parseCommand(char*, void *, int, int);
public:
	ExecutorItem();
	ExecutorItem(const char *exec);
	~ExecutorItem(){};
    const char* getExec();
	bool getParsed(void *, char*);
	virtual CONTENT_TYPES getType();

    virtual bool isValid();
};



#endif //__Content_H_
