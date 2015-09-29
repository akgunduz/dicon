//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Tools_H_
#define __Tools_H_

#include "Log.h"

class Tools {
public:
	static std::string getIPStr(long);
	static std::string getAddressStr(long);
	static INTERFACES getInterface(long);
	static std::string getMD5Str(uint8_t *);
	static std::string hex2str(const uint8_t *ca, int len);
	static bool str2hex(uint8_t *buf, const char *s, uint32_t len);
    static void mkpath(const char *dir);
};

class DiffTime {

public:

	bool initiated = false;
	double tStart;
	double tStop;

	void start();
	double stop();
	void reset();
	bool isInitiated();
};


#endif //__Tools_H_
