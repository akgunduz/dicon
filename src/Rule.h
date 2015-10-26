//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Rule_H_
#define __Rule_H_

#include "Log.h"
#include "FileContent.h"
#include "Unit.h"
#include "JsonFile.h"

class Rule : public JsonFile {

	bool parallel;

	static bool parseRunTypeNode(void*, json_object *node);
    static bool parseFileNode(void*, json_object *node);
    static bool parseParamNode(void*, json_object *node);
    static bool parseExecutorNode(void*, json_object *node);

public:

	Rule(Unit host, Unit node, const char*, FileContent* content = nullptr);
	~Rule();
	bool isParallel();
	void display();

    virtual const char *getFileName();

    virtual FILETYPE getFileType();
};


#endif //__Rule_H_
