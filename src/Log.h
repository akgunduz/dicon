//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Log_H_
#define __Log_H_

#include "Common.h"

enum LOGLEVEL {
	LEVEL_NONE,
	LEVEL_ERROR,
	LEVEL_WARN,
	LEVEL_INFO,
	LEVEL_TRACE,
	LEVEL_ASSERT,
	LEVEL_MAX
};

#define LOG_S(a, ...) Log::display(a, ##__VA_ARGS__)
#define LOG_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOG_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOG_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOG_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, a, ##__VA_ARGS__)
#define LOG_A(c, a, ...) if (!(c)) { Log::log(LEVEL_ASSERT, __FILE__, __LINE__, a, ##__VA_ARGS__);\
				char *(_do_crash) = NULL; *(_do_crash) = 1;} assert(c)

#define LOG_U Log::display_at_ui
//#define LOG_U(a, b) Log::display_at_ui(a, b)
//#define LOG_UI(a, b, c) Log::display_at_ui(a, b, c)

class Log {
	static LOGLEVEL mLevel;
public:

	static void setLogLevel(LOGLEVEL level);
	static void iterateLogLevel();
	static void log(LOGLEVEL level, const char *file, int line, const char *format, ...);
	static void display(const char *format, ...);

	static void display_at_ui(int, uint64_t);
	static void display_at_ui(int, uint64_t, uint64_t);
	static void display_at_ui(int, uint64_t, const char* , ...);
	static void display_at_ui(int, uint64_t, uint64_t, const char* , ...);
	static void display_at_ui(int, const char* , ...);

	static void display_at_ui(int, void*);
	static void update_ui(int, void*);
};

#endif //__Log_H_
