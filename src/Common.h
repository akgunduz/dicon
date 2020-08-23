//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_COMMON_H
#define DICON_COMMON_H

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <ctime>

#include <iosfwd>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <stdexcept>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <filesystem>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef __APPLE__
#include "Common_apple.h"
#elif WIN32
#else
#include "Common_linux.h"
#endif

#ifndef nullptr
#define nullptr NULL
#endif

#define BUFFER_SIZE 512

#define SHUTDOWN_NOTIFIER 'Q'

struct char_cmp {
	bool operator () (const std::string &a,const std::string &b) const
	{
		return a.compare(b) < 0;
	}
};

#endif //DICON_COMMON_H
