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
#include <fcntl.h>

#ifdef __APPLE__
#include "Common_apple.h"
#elif WIN32
#include "Common_windows.h"
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
