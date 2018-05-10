//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Util_H_
#define __Util_H_

#include "Log.h"
#include "ComponentObject.h"

#define ROOT_SIGN "@@@root@@@"

struct cmp_uuid {
	bool operator()(uuid_t *a, uuid_t *b) const {
		return memcmp(a, b, sizeof(uuid_t)) < 0;
	}
};

typedef std::array<unsigned char, sizeof(uuid_t)> TypeUUID;

class Util {

private:
	static std::string getPath(const char *, const char*, const char*, bool);

public:
	static std::string hex2str(const uint8_t *ca, int len);
	static bool str2hex(uint8_t *buf, const char *s, uint32_t len);
	static void mkPath(const char *dir);
	static void removePath(const char *path);
	static std::string extractFile(const char *);
    static std::vector<std::string> getFileList(const char*, const char*);
    static std::vector<std::string> getDirList(const char*, const char*);
	static std::string mixPath(const char*, const char*);
	static std::string parsePath(const char *, const char *);
	static std::string getAbsPath(const char *, const char *);
	static std::string getAbsRefPath(const char *, const char*, const char*);
	static std::string getAbsMD5Path(const char *, const char*, const char*);
	static std::string getRefPath(const char *, const char*, const char*);
	static std::string getMD5Path(const char *, const char*, const char*);
	static bool checkPath(const char*, bool);
	static bool checkPath(const char*, const char*, bool);
	static bool checkPath(const char*, const char*, const char*, bool);
    static bool isMulticast();
	static void cleanup();

};


#endif //__Tools_H_
