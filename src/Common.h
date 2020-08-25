//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_COMMON_H
#define DICON_COMMON_H

#if defined(__APPLE__)
#include "Common_apple.h"
#elif defined(WIN32)
#include "Common_windows.h"
#else
#include "Common_linux.h"
#endif

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
#include <sstream>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <stdexcept>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <random>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 512

#define SHUTDOWN_NOTIFIER 'Q'

struct char_cmp {
	bool operator () (const std::string &a,const std::string &b) const
	{
		return a.compare(b) < 0;
	}
};

#endif //DICON_COMMON_H
