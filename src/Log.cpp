//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "ComponentTypes.h"
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

void Log::log(LOGLEVEL level, const char *file, int line, LOGTYPE type, ...) {

	if (logLevel < level) {
		return;
	}

    char buf[PATH_MAX];
    char extra[PATH_MAX];
    char logout[PATH_MAX];

    strcpy(logout, "");

    std::string fileName = Util::extractFile(file);
    sprintf(extra, "%s : %s[%d]:", sLogLevels[level], fileName.c_str(), line);

#ifndef DISABLE_LOGFILEINFO
    sprintf(logout, "%s %s", extra, logout);
#endif

    va_list ap;
    va_start(ap, type);

    switch(type) {

        case LOG_DUMP: {
            char * fmt = va_arg(ap, char *);
            vsnprintf(buf, sizeof(buf), fmt, ap);
            sprintf(logout, "%s \n", buf);
        }
            break;

        case LOG_STD: {
            COMPONENT host = (COMPONENT)va_arg(ap, int);
            int hostID = va_arg(ap, int);
            char * fmt = va_arg(ap, char *);
            vsnprintf(buf, sizeof(buf), fmt, ap);
            sprintf(logout, "%11s[%d] : %s \n",
                    ComponentTypes::getName(host),
                    hostID,
                    buf);
        }
            break;

        case LOG_COMM: {
            COMPONENT host = (COMPONENT)va_arg(ap, int);
            int hostID = va_arg(ap, int);
            COMPONENT target = (COMPONENT)va_arg(ap, int);
            int targetID = va_arg(ap, int);
            bool direction = (bool) va_arg(ap, int);
            char * fmt = va_arg(ap, char *);
            vsnprintf(buf, sizeof(buf), fmt, ap);
            sprintf(logout, "%11s[%d] %s %11s[%d] : %s \n",
                    ComponentTypes::getName(host),
                    hostID,
                    direction ? "==>" : "<==",
                    ComponentTypes::getName(target),
                    targetID,
                    buf);
        }
            break;

        default:
            break;
    }

    va_end(ap);

    printf("%s", logout);
}

UserInterfaceController *Log::getController() {

    return controller;
}
