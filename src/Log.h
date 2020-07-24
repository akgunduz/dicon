//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_LOG_H
#define DICON_LOG_H

#include "ComponentObject.h"

enum LOGLEVEL {

	LEVEL_NONE,
	LEVEL_ERROR,
	LEVEL_WARN,
	LEVEL_INFO,
	LEVEL_DEBUG,
	LEVEL_TRACE,
	LEVEL_ASSERT,
	LEVEL_MAX
};

enum LOGTYPE {

    LOG_DUMP,
    LOG_STD,
    LOG_COMM,
    LOG_MAX
};

#define LOG_A(c, a, ...) if (!(c)) { Log::log(LEVEL_ASSERT, __FILE__, __LINE__, a, ##__VA_ARGS__);\
				char *(_do_crash) = NULL; *(_do_crash) = 1;} assert(c)
#define PRINT(a, ...) Log::show(a, ##__VA_ARGS__)

#define LOGC_E(a, b, c, ...) Log::logc(LEVEL_ERROR, __FILE__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_W(a, b, c, ...) Log::logc(LEVEL_WARN, __FILE__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_I(a, b, c, ...) Log::logc(LEVEL_INFO, __FILE__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_D(a, b, c, ...) Log::logc(LEVEL_DEBUG, __FILE__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_T(a, b, c, ...) Log::logc(LEVEL_TRACE, __FILE__, __LINE__, a, b, c, ##__VA_ARGS__)

#define LOGS_E(a, ...) Log::logs(LEVEL_ERROR, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_W(a, ...) Log::logs(LEVEL_WARN, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_I(a, ...) Log::logs(LEVEL_INFO, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_D(a, ...) Log::logs(LEVEL_DEBUG, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_T(a, ...) Log::logs(LEVEL_TRACE, __FILE__, __LINE__, a, ##__VA_ARGS__)

class Log {

	static LOGLEVEL logLevel[2];

public:

    static void init(LOGLEVEL, LOGLEVEL);
	static void setLogLevel(LOGLEVEL, LOGLEVEL);
    static void logs(LOGLEVEL level, const char *, int, const ComponentObject, ...);
    static void logc(LOGLEVEL level, const char *, int, const ComponentObject, const ComponentObject&, int, ...);
	static void show(const char *format, ...);
};

#endif // DICON_LOG_H
