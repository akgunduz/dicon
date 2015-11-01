//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ExecutorContent_H_
#define __ExecutorContent_H_

#include "ContentItem.h"

class ExecutorItem : public ContentItem {
	std::string exec;
	bool parseCommand(char*, void *, int, int);
public:
	ExecutorItem();
	ExecutorItem(const char *exec);
	~ExecutorItem(){};
	std::string getExec();
	std::string getParsed(void *);
	void setExec(const std::string&);
	virtual CONTENT_TYPES getType();
};



#endif //__Content_H_
