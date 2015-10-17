//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __ExecutorContent_H_
#define __ExecutorContent_H_

#include "Content.h"

class ExecutorContent : public Content {
	std::string exec;
	bool parseCommand(std::string&, void *, int, int);
public:
	ExecutorContent(){};
	ExecutorContent(const char *exec);
	~ExecutorContent(){};
	std::string getExec();
	std::string getParsed(void *);
	void setExec(const std::string&);
	virtual CONTENT_TYPES getType();
};



#endif //__Content_H_
