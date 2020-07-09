//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "ComponentObject.h"
#include "Util.h"

LOGLEVEL Log::logLevel[] = {LEVEL_ERROR, LEVEL_ERROR};

static const char* sLogLevels[LEVEL_MAX] = {
		"NONE  ",
		"ERROR ",
		"WARN  ",
		"INFO  ",
		"TRACE ",
		"ASSERT",
};

void Log::init(LOGLEVEL stdLevel, LOGLEVEL commLevel) {

    setLogLevel(stdLevel, commLevel);
}

void Log::setLogLevel(LOGLEVEL stdLevel, LOGLEVEL commLevel) {

	logLevel[0] = stdLevel;
	logLevel[1] = commLevel;

	setbuf(stdout, nullptr);

	printf("Log Level Set to Std : %s, Comm : %s\n", sLogLevels[stdLevel], sLogLevels[commLevel]);
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
               const ComponentObject& host, ...) {

    if (logLevel[0] < level) {
        return;
    }

    char buf[PATH_MAX - 20];
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
    sprintf(logout, "%11s[%d] : %s \n",
            host.getName(),
            host.getID(),
            buf);

    va_end(ap);

    printf("%s", logout);
}

void Log::logc(LOGLEVEL level, const char *file, int line,
               const ComponentObject& host, const ComponentObject& target, int direction, ...) {

    if (logLevel[1] < level) {
        return;
    }

    char buf[PATH_MAX - 40];
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
    sprintf(logout, "%11s[%d] %s %11s[%d] : %s \n",
            host.getName(),
            host.getID(),
            direction ? "==>" : "<==",
            target.getName(),
            target.getID(),
            buf);

    va_end(ap);

    printf("%s", logout);

}
