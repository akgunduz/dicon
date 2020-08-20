//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_UTIL_H
#define DICON_UTIL_H

#include "Common.h"

#define BUFFER_SIZE 512

#define ROOT_SIGN "@@@root@@@"

class Util {

	static std::string getPath(const char *, long, const char*, bool);

public:

	static std::string hex2str(const uint8_t *, int);
	static bool str2hex(uint8_t *, const char *, uint32_t);
    static void replaceStr(std::string&, const std::string&, const std::string&);
    static void mkPath(const char *);
	static void removePath(const char *);
	static std::string extractFile(const char *);
    static std::vector<std::string> getFileList(const char*, const char*);
    static std::vector<std::string> getDirList(const char*, const char*);
	static std::string mixPath(const char*, const char*);
	static std::string parsePath(const char *, const char *);
	static std::string getAbsPath(const char *, const char *);
	static std::string getAbsRefPath(const char *, long, const char*);
	static std::string getRefPath(const char *, long, const char*);
	static bool checkPath(const char*, bool);
	static bool checkPath(const char*, const char*, bool);
	static bool checkPath(const char*, long, const char*, bool);
    static bool isMulticast();
	static void init();
	static bool extractZipFile(const char*, const char*);

    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

};

#endif //DICON_UTIL_H
