//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Log_H_
#define __Log_H_

#include <ui/UserInterfaceController.h>

enum LOGLEVEL {

	LEVEL_NONE,
	LEVEL_ERROR,
	LEVEL_WARN,
	LEVEL_INFO,
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

#define LOG_A(c, a, ...) if (!(c)) { Log::log(LEVEL_ASSERT, __FILE__, __LINE__, a, LOG_DUMP, ##__VA_ARGS__);\
				char *(_do_crash) = NULL; *(_do_crash) = 1;} assert(c)
#define LOG_S(a, ...) Log::display(a, ##__VA_ARGS__)
#define LOG_U Log::getController()->display

#define LOG_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)

#define LOGC_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)
#define LOGC_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)
#define LOGC_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)
#define LOGC_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)

#define LOGS_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)
#define LOGS_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)
#define LOGS_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)
#define LOGS_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)

class Log {

	static LOGLEVEL logLevel;

	static UserInterfaceController *controller;

public:

    static void init(LOGLEVEL level, void*, TypeUIUpdateCB);
	static void setLogLevel(LOGLEVEL level);
	static void iterateLogLevel();
	static void log(LOGLEVEL level, const char *, int, LOGTYPE, ...);
	static void display(const char *format, ...);
	static UserInterfaceController* getController();

};

#endif //__Log_H_
