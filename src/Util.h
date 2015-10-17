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
	static short getID();
	static const char* getIDDir(enum ARCHIDS);
	static bool getAbsolutePath(Unit host, Unit node, FILETYPE fileType,
                                const char* rootPath, const char* path, char *absPath, char *md5Path);
};


#endif //__Tools_H_
