//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Util_H_
#define __Util_H_

#include "Log.h"
#include "Unit.h"

class Util {
public:
	static std::string hex2str(const uint8_t *ca, int len);
	static bool str2hex(uint8_t *buf, const char *s, uint32_t len);
    static void mkPath(const char *dir);
	static ARCH getArch();
    static std::vector<std::string> getFileList(const char*, const char*);
    static std::vector<std::string> getDirList(const char*, const char*);
	static std::string mixPath(const char*, const char*);
	static std::string absPath(Unit, const char *);
	static bool checkPath(const char*, const char*, bool);
    static bool isMulticast();
};


#endif //__Tools_H_
