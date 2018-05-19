//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_COMMON_H
#define BANKOR_COMMON_H

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <climits>
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <string>
#include <cassert>
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
#include <csignal>
#include <ctime>
#include <thread>
#include <sys/ioctl.h>
#include <net/if.h>
#include <uuid/uuid.h>


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

struct char_cmp {
	bool operator () (const std::string a,const std::string b) const
	{
		return a.compare(b) < 0;
	}
};

#endif
