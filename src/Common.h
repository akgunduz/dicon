//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Common_H_
#define __Common_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <string>
#include <assert.h>
#include <cstdarg>
#include <cstring>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <complex>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <sys/time.h>
#include <dirent.h>
#include <json.h>
#include <signal.h>
#include <time.h>

#include <sys/ioctl.h>
#include <net/if.h>

#ifdef __APPLE__
#include "Common_apple.h"
#elif WIN32
#else
#include "Common_linux.h"
#endif

#ifndef nullptr
#define nullptr NULL
#endif

#define SHUTDOWN_NOTIFIER 'Q'

enum NODE_STATES {
	IDLE,
	BUSY,
	PREBUSY,
	REMOVE
};

enum FILETYPE {
    FILE_JOB,
	FILE_COMMON,
    FILE_MAX
};

enum UI_UPDATE {
	UI_UPDATE_LOG,

	UI_UPDATE_DIST_ADDRESS,
	UI_UPDATE_DIST_COLL_LIST,
	UI_UPDATE_DIST_NODE_LIST,
	UI_UPDATE_DIST_BACKUP,
	UI_UPDATE_DIST_LOG,

	UI_UPDATE_COLL_ADDRESS,
	UI_UPDATE_COLL_ATT_DIST_ADDRESS,
	UI_UPDATE_COLL_ATT_NODE_ADDRESS,
    UI_UPDATE_COLL_FILE_LIST,
	UI_UPDATE_COLL_PROCESS_LIST,
	UI_UPDATE_COLL_LOG,

	UI_UPDATE_NODE_ADDRESS,
	UI_UPDATE_NODE_STATE,
	UI_UPDATE_NODE_ATT_COLL_ADDRESS,
	UI_UPDATE_NODE_FILE_LIST,
	UI_UPDATE_NODE_PARAM_LIST,
	UI_UPDATE_NODE_EXEC_LIST,
	UI_UPDATE_NODE_CLEAR,
	UI_UPDATE_NODE_LOG,

	UI_UPDATE_MAX
};

class EventData {

public:
	uint64_t data64_1;
	uint64_t data64_2;
	std::string dataStr;
    void *dataPtr;

	EventData(uint64_t data) : data64_1(data) {}
	EventData(uint64_t data, uint64_t data2) : data64_1(data), data64_2(data2) {}
	EventData(uint64_t data, const char *data2) : data64_1(data), dataStr(data2) {}
	EventData(uint64_t data, uint64_t data2, const char *data3) : data64_1(data), data64_2(data2), dataStr(data3) {}
	EventData(const char *data) : dataStr(data) {}
	EventData(void *data) : dataPtr(data) {}
};

extern const char* sStates[];
extern const char* sParamTypes[];

#endif
