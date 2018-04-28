//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "ComponentTypes.h"

LOGLEVEL Log::mLevel = LEVEL_ERROR;
void* Log::uiContext = NULL;
update_ui_callback Log::cb = NULL;

static const char* sLogLevels[LEVEL_MAX] = {
		"NONE  ",
		"ERROR ",
		"WARN  ",
		"INFO  ",
		"TRACE ",
		"ASSERT",
};

void Log::setLogLevel(LOGLEVEL level) {

	mLevel = level;
	setbuf(stdout, NULL);

	printf("Log Level Set to %s\n", sLogLevels[level]);
}

void Log::iterateLogLevel() {

	if (mLevel == LEVEL_TRACE) {
		Log::setLogLevel(LEVEL_ERROR);
	} else {
		Log::setLogLevel(LOGLEVEL((int)mLevel + 1));
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

void Log::display_at_ui(int id, uint64_t data64_1) {

	EventData *data = new EventData(data64_1);

	update_ui(id, data);

}

void Log::display_at_ui(int id, uint64_t data64_1, uint64_t data64_2) {

	EventData *data = new EventData(data64_1, data64_2);

	update_ui(id, data);

}

void Log::display_at_ui(int id, uint64_t data64_1, const char *data64_2, ...) {

	char buf[256];
	va_list ap;
	va_start(ap, data64_2);
	vsnprintf(buf, sizeof(buf), data64_2, ap);
	va_end(ap);

	EventData *data = new EventData(data64_1, buf);

	update_ui(id, data);

}

void Log::display_at_ui(int id, uint64_t data64_1, uint64_t data64_2, char const *data64_3, ...) {

	char buf[256];
	va_list ap;
	va_start(ap, data64_3);
	vsnprintf(buf, sizeof(buf), data64_3, ap);
	va_end(ap);

	EventData *data = new EventData(data64_1, data64_2, buf);

	update_ui(id, data);

}

void Log::display_at_ui(int id, char const *data64_1, ...) {

	char buf[256];
	va_list ap;
	va_start(ap, data64_1);
	vsnprintf(buf, sizeof(buf), data64_1, ap);
	va_end(ap);

	EventData *data = new EventData(buf);

	update_ui(id, data);

}

void Log::display_at_ui(int id, void *data) {

	update_ui(id, data);

}

void Log::log(LOGLEVEL level, const char *file, int line, LOGTYPE type, ...) {

	if (mLevel < level) {
		return;
	}

    char buf[PATH_MAX];
    char extra[PATH_MAX];
    char logout[PATH_MAX];

    strcpy(logout, "");

    std::string fileName = extractFile(file);
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
            bool direction = va_arg(ap, bool);
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

void Log::set_ui_callback(void *context, update_ui_callback cb) {

    Log::uiContext = context;
	Log::cb = cb;
}

void Log::update_ui(int id, void *data) {

	if (cb && uiContext) {
		cb(uiContext, id, data);
	}
}

std::string Log::extractFile(const char *path) {

    std::string file = path;
    size_t pos = file.find_last_of('/');
    file = file.substr(pos + 1);
    pos = file.find_first_of('.');
    return file.substr(0, pos);
}


