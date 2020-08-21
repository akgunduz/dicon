//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "HostUnit.h"
#include "Util.h"

LOGLEVEL Log::logLevel[] = {LEVEL_ERROR, LEVEL_ERROR};

static const char* sColorCodes[COLOR_MAX] = {
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

static LogLevel sLogLevels[LEVEL_MAX] = {
        {"NONE  ", sColorCodes[COLOR_RESET]},
        {"ERROR ", sColorCodes[COLOR_RED]},
        {"WARN  ", sColorCodes[COLOR_BLUE]},
        {"INFO  ", sColorCodes[COLOR_GREEN]},
        {"DEBUG  ", sColorCodes[COLOR_RESET]},
        {"TRACE  ", sColorCodes[COLOR_RESET]},
        {"ASSERT  ", sColorCodes[COLOR_RESET]},
};

void Log::init(LOGLEVEL stdLevel, LOGLEVEL commLevel) {

    setLogLevel(stdLevel, commLevel);
}

void Log::setLogLevel(LOGLEVEL stdLevel, LOGLEVEL commLevel) {

	logLevel[0] = stdLevel;
	logLevel[1] = commLevel;

	setbuf(stdout, nullptr);

	printf("Log Level Set to Std : %s, Comm : %s\n", sLogLevels[stdLevel].name, sLogLevels[commLevel].name);
}

void Log::show(const char *format, ...) {

	char buf[256];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	printf("%s \n", buf);

}

void Log::logs(LOGLEVEL level, const char *file, int line,
               const HostUnit host, ...) {

    if (logLevel[0] < level) {
        return;
    }

    char buf[PATH_MAX - NAME_MAX];
    char logout[PATH_MAX];

    strcpy(logout, "");

#ifndef DISABLE_LOGFILEINFO
    char extra[PATH_MAX];
    std::string fileName = Util::extractFile(file);
    sprintf(extra, "%s : %s[%d]:", sLogLevels[level], fileName.c_str(), line);
    sprintf(logout, "%s %s", extra, logout);
#endif

    va_list ap;
    va_start(ap, host);

    char * fmt = va_arg(ap, char *);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    sprintf(logout, "%11s[%ld]                    : %s \n",
            ComponentType::getName(host.getType()),
            host.getID(),
            buf);

    va_end(ap);
    printf("\033%s%s\033%s", sLogLevels[level].color, logout, sColorCodes[COLOR_RESET]);
}

void Log::logc(LOGLEVEL level, const char *file, int line,
               const HostUnit host, const ComponentUnit& target, MSG_DIR direction, ...) {

    if (logLevel[1] < level) {
        return;
    }

    char buf[PATH_MAX - NAME_MAX];
    char logout[PATH_MAX];

    strcpy(logout, "");


#ifndef DISABLE_LOGFILEINFO
    char extra[PATH_MAX];
    std::string fileName = Util::extractFile(file);
    sprintf(extra, "%s : %s[%d]:", sLogLevels[level], fileName.c_str(), line);
    sprintf(logout, "%s %s", extra, logout);
#endif

    va_list ap;
    va_start(ap, direction);

    char * fmt = va_arg(ap, char *);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    sprintf(logout, "%11s[%ld] %s %11s[%ld] : %s \n",
            ComponentType::getName(host.getType()),
            host.getID(),
            MessageType::getMsgDirName(direction),
            ComponentType::getName(target.getType()),
            target.getID(),
            buf);

    va_end(ap);

    printf("\033%s%s\033%s", sLogLevels[level].color, logout, sColorCodes[COLOR_RESET]);
}
