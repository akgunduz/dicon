//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_LOG_H
#define DICON_LOG_H

#include "HostUnit.h"
#include "MessageType.h"

#define LOG_DIST (1u << COMP_DISTRIBUTOR)
#define LOG_COLL (1u << COMP_COLLECTOR)
#define LOG_NODE (1u << COMP_NODE)
#define LOG_ALL LOG_DIST | LOG_COLL | LOG_NODE

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

struct LogInfo {

    LOGLEVEL level;
    uint8_t filter;
    bool timeStamp;
};

struct LogLevel {

    const char* name;
    const char* color;
};

class Log {

    static inline LogInfo logInfo = {LEVEL_INFO, LOG_ALL, false};

    static inline UtilTime timer;

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

    static inline bool checkLevel(LOGLEVEL level) {

        return logInfo.level >= level;
    }

    static inline bool checkFilter(COMPONENT type) {

        return (1u << type) & logInfo.filter;
    }

public:

    static void init(const LogInfo& _logInfo) {

        logInfo = _logInfo;

        setbuf(stdout, nullptr);

        printf("Log Level Set to %s\n", sLogLevels[logInfo.level].name);
    };

	static inline void _logP(LOGLEVEL level, const char* logout) {

        logInfo.timeStamp ?
        printf("\033%sT : %" PRIu64 ", %s\033%s\n", sLogLevels[level].color, timer.duration(), logout, sColorCodes[COLOR_RESET])
        :
        printf("\033%s%s\033%s\n", sLogLevels[level].color, logout, sColorCodes[COLOR_RESET]);
	}

    static void logP(LOGLEVEL level, const char* format) {

        if (!checkLevel(level)) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, "%s", format);

        _logP(level, logout);
    }

    template<typename... Args>
    static void logP(LOGLEVEL level, const char* format, Args&&... args) {

        if (!checkLevel(level)) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, format, args...);

        _logP(level, logout);
    }

    static inline void _logS(LOGLEVEL level, const TypeHostUnit& host, const char* logout) {

        logInfo.timeStamp ?
        printf("\033%sT : %" PRIu64 ", %11s[%d]                    : %s\033%s\n",
               sLogLevels[level].color,
               timer.duration(),
               ComponentType::getName(host->getType()),
               host->getID(),
               logout,
               sColorCodes[COLOR_RESET])
       :
        printf("\033%s%11s[%d]                    : %s\033%s\n",
               sLogLevels[level].color,
               ComponentType::getName(host->getType()),
               host->getID(),
               logout,
               sColorCodes[COLOR_RESET]);
    }

    static void logS(LOGLEVEL level, const TypeHostUnit& host, const char* format) {

        if (!checkLevel(level) || !checkFilter(host->getType())) {
            return;
        }

        char logout[PATH_MAX];
        std::snprintf(logout, PATH_MAX - 1, "%s", format);

        _logS(level, host, logout);
    }

    template<typename... Args>
    static void logS(LOGLEVEL level, const TypeHostUnit& host, const char* format, Args&&... args) {

        if (!checkLevel(level) || !checkFilter(host->getType())) {
            return;
        }

        char logout[PATH_MAX];
        std::snprintf(logout, PATH_MAX - 1, format, args...);

        _logS(level, host, logout);
    }

    static inline void _logC(LOGLEVEL level, const TypeHostUnit& host,
                             const TypeCommUnit& target, MSG_DIR direction,
                             const char* logout) {

	    logInfo.timeStamp ?
        printf("\033%sT : %" PRIu64 ", %11s[%d] %s %11s[%d] : %s\033%s\n",
               sLogLevels[level].color,
               timer.duration(),
               ComponentType::getName(host->getType()),
               host->getID(),
               MessageType::getMsgDirName(direction),
               ComponentType::getName(target->getType()),
               target->getID(),
               logout,
               sColorCodes[COLOR_RESET])
       :
        printf("\033%s%11s[%d] %s %11s[%d] : %s\033%s\n",
               sLogLevels[level].color,
               ComponentType::getName(host->getType()),
               host->getID(),
               MessageType::getMsgDirName(direction),
               ComponentType::getName(target->getType()),
               target->getID(),
               logout,
               sColorCodes[COLOR_RESET]);
    }

    static void logC(LOGLEVEL level, const TypeHostUnit& host,
                     const TypeCommUnit& target, MSG_DIR direction,
                     const char* format) {

        if (!checkLevel(level) || !checkFilter(host->getType())) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, "%s", format);

        _logC(level, host, target, direction, logout);
    }

    template<typename... Args>
    static void logC(LOGLEVEL level, const TypeHostUnit& host,
                     const TypeCommUnit& target, MSG_DIR direction,
                     const char* format, Args&&... args) {

        if (!checkLevel(level) || !checkFilter(host->getType())) {
            return;
        }

        char logout[PATH_MAX];
        std::sprintf(logout, format, args...);

        _logC(level, host, target, direction, logout);
    }
};

#endif // DICON_LOG_H
