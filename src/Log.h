//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_LOG_H
#define BANKOR_LOG_H

#include <ui/UserInterfaceController.h>
#include "ComponentObject.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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

#define LOG_A(c, a, ...) if (!(c)) { Log::log(LEVEL_ASSERT, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__);\
				char *(_do_crash) = NULL; *(_do_crash) = 1;} assert(c)
#define LOG_S(a, ...) Log::show(a, ##__VA_ARGS__)
#define LOG_U(a, ...) Log::updateUI(a, ##__VA_ARGS__)

//#define LOG_E(a, ...) Log::logd(LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)
//#define LOG_W(a, ...) Log::logd(LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)
//#define LOG_I(a, ...) Log::logd(LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)
//#define LOG_T(a, ...) Log::logd(LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)

#define LOGC_E(a, b, c, ...) Log::logc(LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_W(a, b, c, ...) Log::logc(LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_I(a, b, c, ...) Log::logc(LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, a, b, c, ##__VA_ARGS__)
#define LOGC_T(a, b, c, ...) Log::logc(LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, a, b, c, ##__VA_ARGS__)

#define LOGS_E(a, ...) Log::logs(LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_W(a, ...) Log::logs(LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_I(a, ...) Log::logs(LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)
#define LOGS_T(a, ...) Log::logs(LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, a, ##__VA_ARGS__)

class Log {

	static LOGLEVEL logLevel[2];

	static UserInterfaceController *controller;

public:

    static void init(LOGLEVEL, LOGLEVEL);
    static void registerUIController(void*, TypeUIUpdateCB);
	static void setLogLevel(LOGLEVEL, LOGLEVEL);
	//static void logd(LOGLEVEL level, const char *, int, ...);
    static void logs(LOGLEVEL level, const char *, const char *, int, ComponentObject, ...);
    static void logc(LOGLEVEL level, const char *, const char *, int, ComponentObject, ComponentObject, int, ...);
	static void show(const char *format, ...);

	static void updateUI(int, std::vector<long>);
	static void updateUI(int, std::vector<long>, const char* , ...);
	static void updateUI(int, const char* , ...);
	static void updateUI(int, int, const char* , const char*);
	static void updateUI(int, int, long, const char* , const char*);
	static void updateUI(int, int, const char* , long);
	static void updateUI(int, void*);
	static void updateUI(int, int, void*);

	static UserInterfaceController* getController();

};

#endif //BANKOR_LOG_H
