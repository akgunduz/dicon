//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Common.h"

const char* sArchs[] = {
		"osx",
		"win",
		"lin",
		"arm11",
		"a7",
		"a8",
		"a9",
		"a15",
};

const char* sStates[] = {
		"IDLE",
		"BUSY",
		"PREBUSY"
};

const char* sParamTypes[] = {
		"LONG",
		"DOUBLE",
		"STRING",
};

const char* sInterfaces[] = {
		"TCP",
		"UnixSocket",
		"Pipe",
};