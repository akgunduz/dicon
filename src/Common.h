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
#include <algorithm>
#include <stdexcept>
#include <condition_variable>
#include <mutex>
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
#include <thread>
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

enum UI_UPDATE {

	UI_UPDATE_DIST_ADDRESS,
	UI_UPDATE_DIST_COLL_LIST,
	UI_UPDATE_DIST_NODE_LIST,
	UI_UPDATE_DIST_BACKUP,

	UI_UPDATE_COLL_ADDRESS,
	UI_UPDATE_COLL_ATT_DIST_ADDRESS,
	UI_UPDATE_COLL_ATT_NODE_ADDRESS,
    UI_UPDATE_COLL_FILE_LIST,
    UI_UPDATE_COLL_FILE_LISTITEM,
	UI_UPDATE_COLL_PROCESS_LIST,

	UI_UPDATE_NODE_ADDRESS,
	UI_UPDATE_NODE_STATE,
	UI_UPDATE_NODE_ATT_COLL_ADDRESS,
	UI_UPDATE_NODE_FILE_LIST,
	UI_UPDATE_NODE_PARAM_LIST,
	UI_UPDATE_NODE_EXEC_LIST,
	UI_UPDATE_NODE_CLEAR,

	UI_UPDATE_MAX
};

struct char_cmp {
	bool operator () (const std::string a,const std::string b) const
	{
		return a.compare(b) < 0;
	}
};

#endif
