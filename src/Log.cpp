//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "ComponentObject.h"
#include "Util.h"

LOGLEVEL Log::logLevel[] = {LEVEL_ERROR, LEVEL_ERROR};
UserInterfaceController* Log::controller = NULL;

static const char* sLogLevels[LEVEL_MAX] = {
		"NONE  ",
		"ERROR ",
		"WARN  ",
		"INFO  ",
		"TRACE ",
		"ASSERT",
};

void Log::init(LOGLEVEL stdlevel, LOGLEVEL comlevel) {

    setLogLevel(stdlevel, comlevel);
}

void Log::setLogLevel(LOGLEVEL stdlevel, LOGLEVEL comlevel) {

	logLevel[0] = stdlevel;
	logLevel[1] = comlevel;

	setbuf(stdout, NULL);

	printf("Log Level Set to Std : %s, Comm : %s\n", sLogLevels[stdlevel], sLogLevels[comlevel]);
}

void Log::show(const char *format, ...) {

	char buf[256];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	printf("%s \n", buf);

}

//void Log::logd(LOGLEVEL level, const char *file, int line,
//              ...) {
//
//    if (logLevel < level) {
//        return;
//    }
//
//    char buf[PATH_MAX];
//    char logout[PATH_MAX];
//
//    strcpy(logout, "");
//
//#ifndef DISABLE_LOGFILEINFO
//    char extra[PATH_MAX];
//    std::string fileName = Util::extractFile(file);
//    sprintf(extra, "%s : %s[%d]:", sLogLevels[level], fileName.c_str(), line);
//    sprintf(logout, "%s %s", extra, logout);
//#endif
//
//    va_list ap;
//    va_start(ap, line);
//
//    char * fmt = va_arg(ap, char *);
//    vsnprintf(buf, sizeof(buf), fmt, ap);
//    sprintf(logout, "%14s : %s \n", "Common", buf);
//
//    va_end(ap);
//
//    printf("%s", logout);
//}

void Log::logs(LOGLEVEL level, const char *file, int line,
               ComponentObject host, ...) {

    if (logLevel[0] < level) {
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

    if (logLevel[1] < level) {
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

void Log::registerUIController(void *context, TypeUIUpdateCB updateCB) {

    controller = UserInterfaceController::newInstance(context, updateCB);
}

void Log::updateUI(int id, std::vector<long> dataList) {

    if (controller) {
        controller->display(id, dataList);
    }
}


void Log::updateUI(int id, char const *data1, ...) {

    if (controller) {
        controller->display(id, data1);
    }
}

void Log::updateUI(int id, int data1, char const *data2, char const *data3) {

    if (controller) {
        controller->display(id, data1, data2, data3);
    }
}

void Log::updateUI(int id, int data1, char const *data2, long data3) {

    if (controller) {
        controller->display(id, data1, data2, data3);
    }
}

void Log::updateUI(int id, int data1, long data2, char const *data3, char const *data4) {

    if (controller) {
        controller->display(id, data1, data2, data3, data4);
    }
}

void Log::updateUI(int id, std::vector<long> data1, const char *data2, ...) {

    if (controller) {
        controller->display(id, data1, data2);
    }
}

void Log::updateUI(int id, void *data1) {

    if (controller) {
        controller->display(id, data1);
    }
}

void Log::updateUI(int id, int data1, void *data2) {

    if (controller) {
        controller->display(id, data1, data2);
    }
}
