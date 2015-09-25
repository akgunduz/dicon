//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"

#ifdef USE_WXWIDGETS
#include "UserInterface.h"
extern UserInterface *g_ui;
#endif

#include "Console.h"

LOGLEVEL Log::mLevel = LEVEL_ERROR;

extern Console *c_ui;

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

void Log::log(LOGLEVEL level, const char *file, int line, const char *format, ...) {

	if (mLevel < level) {
		return;
	}

	char buf[256];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	char rfile[255];
	strcpy(rfile, file);
	char *p = strrchr(rfile, '/');
	if (!p) {
		p = rfile;
	} else {
		p += 1;
	}
	char *d = strchr(p, '.');
	if (d) {
		*d = '\0';
	}

	char logout[255];
	sprintf(logout, "%s : %s[%d]: %s \n", sLogLevels[level], p, line, buf);

	EventData *data = new EventData(logout);

	update_ui(UI_UPDATE_LOG, data);

}

void Log::update_ui(int id, void *data) {

#ifdef USE_WXWIDGETS
	if (g_ui) {
		g_ui->updateUIEvent(id, data);
	}
#endif

	if (c_ui) {
		c_ui->updateConsoleEvent(id, data);
	}
}



