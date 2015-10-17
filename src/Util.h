//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Util_H_
#define __Util_H_

#include "Log.h"

class Util {
public:
	static std::string hex2str(const uint8_t *ca, int len);
	static bool str2hex(uint8_t *buf, const char *s, uint32_t len);
    static void mkPath(const char *dir);
};


#endif //__Tools_H_
