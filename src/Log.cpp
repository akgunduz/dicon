//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "ComponentObject.h"
#include "Util.h"

LOGLEVEL Log::logLevel = LEVEL_ERROR;
UserInterfaceController* Log::controller = NULL;

static const char* sLogLevels[LEVEL_MAX] = {
		"NONE  ",
		"ERROR ",
		"WARN  ",
		"INFO  ",
		"TRACE ",
		"ASSERT",
};

void Log::init(LOGLEVEL level, void *context, TypeUIUpdateCB updateCB) {

    controller = UserInterfaceController::newInstance(context, updateCB);

    setLogLevel(level);
}

void Log::setLogLevel(LOGLEVEL level) {

	logLevel = level;

	setbuf(stdout, NULL);

	printf("Log Level Set to %s\n", sLogLevels[level]);
}

void Log::iterateLogLevel() {

	if (logLevel == LEVEL_TRACE) {

		Log::setLogLevel(LEVEL_ERROR);

	} else {

		Log::setLogLevel(LOGLEVEL((int)logLevel + 1));

	}
}

void Log::display(const char *format, ...) {

	char buf[256];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	printf("%s \n", buf);

}

void Log::logd(LOGLEVEL level, const char *file, int line,
              ...) {

    if (logLevel < level) {
        return;
    }

    char buf[PATH_MAX];
    char logout[PATH_MAX];

    strcpy(logout, "");

#ifndef DISABLE_LOGFILEINFO
    char extra[PATH_MAX];
    std::string fileName = Util::extractFile(file);
    sprintf(extra, "%s : %s[%d]:", sLogLevels[level], fileName.c_str(), line);
    sprintf(logout, "%s %s", extra, logout);
#endif

    va_list ap;
    va_start(ap, line);

    char * fmt = va_arg(ap, char *);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    sprintf(logout, "%14s : %s \n", "Common", buf);

    va_end(ap);

    printf("%s", logout);
}

void Log::logs(LOGLEVEL level, const char *file, int line,
               ComponentObject host, ...) {

    if (logLevel < level) {
        return;
    }

    char buf[PATH_MAX];
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
               ComponentObject host, ComponentObject target, int direction, ...) {

    if (logLevel < level) {
        return;
    }

    char buf[PATH_MAX];
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



UserInterfaceController *Log::getController() {

    return controller;
}
