//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Rule_H_
#define __Rule_H_

#include "Log.h"
#include "FileItem.h"
#include "Unit.h"
#include "JsonItem.h"

class Rule : public JsonItem {

	bool parallel;

    bool active;
    int repeat;

    void init();

	static bool parseRunTypeNode(void*, json_object *node);
    static bool parseFileNode(void*, json_object *node);
    static bool parseParamNode(void*, json_object *node);
    static bool parseExecutorNode(void*, json_object *node);

public:

	Rule(FileItem * content);
	Rule(const char*, const char*);
	~Rule();

	bool isParallel();

    bool getActive();
    void setActive(bool active);

    int getRepeat();
    void setRepeat(int repeat);
};


#endif //__Rule_H_
