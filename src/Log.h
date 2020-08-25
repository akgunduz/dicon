//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_LOG_H
#define DICON_LOG_H

#include "HostUnit.h"
#include "MessageType.h"

#define LOGP_E(a, ...) Log::logP(LEVEL_ERROR, a, ##__VA_ARGS__)
#define LOGP_W(a, ...) Log::logP(LEVEL_WARN, a, ##__VA_ARGS__)
#define LOGP_I(a, ...) Log::logP(LEVEL_INFO, a, ##__VA_ARGS__)
#define LOGP_D(a, ...) Log::logP(LEVEL_DEBUG, a, ##__VA_ARGS__)
#define LOGP_T(a, ...) Log::logP(LEVEL_TRACE, a, ##__VA_ARGS__)

#define LOGS_E(a, b, ...) Log::logS(LEVEL_ERROR, a, b, ##__VA_ARGS__)
#define LOGS_W(a, b, ...) Log::logS(LEVEL_WARN, a, b, ##__VA_ARGS__)
#define LOGS_I(a, b, ...) Log::logS(LEVEL_INFO, a, b, ##__VA_ARGS__)
#define LOGS_D(a, b, ...) Log::logS(LEVEL_DEBUG, a, b, ##__VA_ARGS__)
#define LOGS_T(a, b, ...) Log::logS(LEVEL_TRACE, a, b, ##__VA_ARGS__)

#define LOGC_E(a, b, c, d, ...) Log::logC(LEVEL_ERROR, a, b, c, d, ##__VA_ARGS__)
#define LOGC_W(a, b, c, d, ...) Log::logC(LEVEL_WARN, a, b, c, d, ##__VA_ARGS__)
#define LOGC_I(a, b, c, d, ...) Log::logC(LEVEL_INFO, a, b, c, d, ##__VA_ARGS__)
#define LOGC_D(a, b, c, d, ...) Log::logC(LEVEL_DEBUG, a, b, c, d, ##__VA_ARGS__)
#define LOGC_T(a, b, c, d, ...) Log::logC(LEVEL_TRACE, a, b, c, d, ##__VA_ARGS__)

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

enum PRINT_COLOR {
    COLOR_RESET,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_MAX
};

struct LogLevel {

    const char* name;
    const char* color;
};

class Log {

	inline static LOGLEVEL logLevel = LEVEL_INFO;

    static constexpr const char* sColorCodes[COLOR_MAX] = {
            "[0m",
            "[0;30m",
            "[0;31m",
            "[0;32m",
            "[0;33m",
            "[0;34m",
            "[0;35m",
            "[0;36m",
            "[0;37m",
    };

    static constexpr LogLevel sLogLevels[LEVEL_MAX] = {
            {"NONE  ", sColorCodes[COLOR_RESET]},
            {"ERROR ", sColorCodes[COLOR_RED]},
            {"WARN  ", sColorCodes[COLOR_BLUE]},
            {"INFO  ", sColorCodes[COLOR_GREEN]},
            {"DEBUG  ", sColorCodes[COLOR_RESET]},
            {"TRACE  ", sColorCodes[COLOR_RESET]},
            {"ASSERT  ", sColorCodes[COLOR_RESET]},
    };

public:

    static void init(LOGLEVEL level) {

        setLogLevel(level);
    };

	static void setLogLevel(LOGLEVEL level) {

        logLevel = level;

        setbuf(stdout, nullptr);

        printf("Log Level Set to %s\n", sLogLevels[level].name);
	};

    static void logP(LOGLEVEL level, const char* format) {

        if (logLevel < level) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, "%s", format);

        printf("\033%s%s\033%s\n", sLogLevels[level].color, logout, sColorCodes[COLOR_RESET]);
    }

    template<typename... Args>
    static void logP(LOGLEVEL level, const char* format, Args&&... args) {

        if (logLevel < level) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, format, args...);

        printf("\033%s%s\033%s\n", sLogLevels[level].color, logout, sColorCodes[COLOR_RESET]);
    }

    static void logS(LOGLEVEL level, const TypeHostUnit& host, const char* format) {

        if (logLevel < level) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, "%s", format);

        printf("\033%s%11s[%ld]                    : %s\033%s\n",
               sLogLevels[level].color,
               ComponentType::getName(host->getType()),
               host->getID(),
               logout,
               sColorCodes[COLOR_RESET]);
    }

    template<typename... Args>
    static void logS(LOGLEVEL level, const TypeHostUnit& host, const char* format, Args&&... args) {

        if (logLevel < level) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, format, args...);

        printf("\033%s%11s[%ld]                    : %s\033%s\n",
               sLogLevels[level].color,
               ComponentType::getName(host->getType()),
               host->getID(),
               logout,
               sColorCodes[COLOR_RESET]);
    }

    static void logC(LOGLEVEL level, const TypeHostUnit& host,
                     const TypeCommUnit& target, MSG_DIR direction,
                     const char* format) {

        if (logLevel < level) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, "%s", format);

        printf("\033%s%11s[%ld] %s %11s[%ld] : %s\033%s\n",
               sLogLevels[level].color,
               ComponentType::getName(host->getType()),
               host->getID(),
               MessageType::getMsgDirName(direction),
               ComponentType::getName(target->getType()),
               target->getID(),
               logout,
               sColorCodes[COLOR_RESET]);
    }

    template<typename... Args>
    static void logC(LOGLEVEL level, const TypeHostUnit& host,
                     const TypeCommUnit& target, MSG_DIR direction,
                     const char* format, Args&&... args) {

        if (logLevel < level) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, format, args...);

        printf("\033%s%11s[%ld] %s %11s[%ld] : %s\033%s\n",
               sLogLevels[level].color,
               ComponentType::getName(host->getType()),
               host->getID(),
               MessageType::getMsgDirName(direction),
               ComponentType::getName(target->getType()),
               target->getID(),
               logout,
               sColorCodes[COLOR_RESET]);
    }
};

#endif // DICON_LOG_H
