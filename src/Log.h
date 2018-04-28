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

enum LOGTYPE {
    LOG_DUMP,
    LOG_STD,
    LOG_COMM,
    LOGTYPE_MAX
};

#define LOG_S(a, ...) Log::display(a, ##__VA_ARGS__)
#define LOG_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, LOG_DUMP, a, ##__VA_ARGS__)
#define LOG_A(c, a, ...) if (!(c)) { Log::log(LEVEL_ASSERT, __FILE__, __LINE__, a, LOG_DUMP, ##__VA_ARGS__);\
				char *(_do_crash) = NULL; *(_do_crash) = 1;} assert(c)

#define LOGC_S(a, ...) Log::display(a, ##__VA_ARGS__)
#define LOGC_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)
#define LOGC_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)
#define LOGC_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)
#define LOGC_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, LOG_COMM, a, ##__VA_ARGS__)

#define LOGS_S(a, ...) Log::display(a, ##__VA_ARGS__)
#define LOGS_E(a, ...) Log::log(LEVEL_ERROR, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)
#define LOGS_W(a, ...) Log::log(LEVEL_WARN, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)
#define LOGS_I(a, ...) Log::log(LEVEL_INFO, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)
#define LOGS_T(a, ...) Log::log(LEVEL_TRACE, __FILE__, __LINE__, LOG_STD, a, ##__VA_ARGS__)

#define LOG_U Log::display_at_ui
//#define LOG_U(a, b) Log::display_at_ui(a, b)
//#define LOG_UI(a, b, c) Log::display_at_ui(a, b, c)

typedef void (*update_ui_callback)(void*, int, void*);

struct EventData {

	uint64_t data64_1;
	uint64_t data64_2;
	std::string dataStr;
	void *dataPtr[3];

	EventData(uint64_t data) : data64_1(data) {}
	EventData(uint64_t data, uint64_t data2) : data64_1(data), data64_2(data2) {}
	EventData(uint64_t data, const char *data2) : data64_1(data), dataStr(data2) {}
	EventData(uint64_t data, uint64_t data2, const char *data3) : data64_1(data), data64_2(data2), dataStr(data3) {}
	EventData(const char *data) : dataStr(data) {}
	EventData(void *data1) {
		dataPtr[0] = data1;
	}
	EventData(void *data1, void *data2) {
		dataPtr[0] = data1;
		dataPtr[1] = data2;
	}
	EventData(void *data1, void *data2, void *data3) {
		dataPtr[0] = data1;
		dataPtr[1] = data2;
		dataPtr[2] = data3;
	}
};

class Log {
	static LOGLEVEL mLevel;
	static update_ui_callback cb;
	static void *uiContext;

	static std::string extractFile(const char* path);
public:

	static void setLogLevel(LOGLEVEL level);
	static void iterateLogLevel();
	static void log(LOGLEVEL level, const char *, int, LOGTYPE, ...);
	static void display(const char *format, ...);

	static void display_at_ui(int, uint64_t);
	static void display_at_ui(int, uint64_t, uint64_t);
	static void display_at_ui(int, uint64_t, const char* , ...);
	static void display_at_ui(int, uint64_t, uint64_t, const char* , ...);
	static void display_at_ui(int, const char* , ...);
	static void display_at_ui(int, void*);

	static void update_ui(int, void*);
	static void set_ui_callback(void *, update_ui_callback cb);
};

#endif //__Log_H_
