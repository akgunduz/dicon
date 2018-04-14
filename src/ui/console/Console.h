//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#include "Common.h"
#include "Collector.h"
#include "Distributor.h"

#ifndef __Console_H_
#define __Console_H_

class Console;

class ConsoleEvent {

	int id;
	uint64_t data64_1;
	uint64_t data64_2;
	std::string dataStr;
	void *dataPointer;

public:
	ConsoleEvent();

	uint64_t GetData1();
	uint64_t GetData2();
	std::string GetString();
	void* GetClientData();

	void SetId(int);
	void SetData1(uint64_t);
	void SetData2(uint64_t);
	void SetString(std::string&);
	void SetClientData(void *);
};

typedef void (Console::*fConsoleUpdater)(ConsoleEvent &event);

class Console {

	Distributor *distObject;
	Collector *collObject;
	Node *nodeObject;

	fConsoleUpdater uiUpdater[UI_UPDATE_MAX];

public:
	Console();

	static void updateUICallback(void*, int, void*);

	void updateUIEvent(int, void*);

	void updateLog(ConsoleEvent &event);

	bool nodeInit(int, int);
	void nodeDestroy();
	void nodeRemoveDir(const char *);
	void nodeRun(int, int);
	void nodeUpdateAddresses(ConsoleEvent &event);
	void nodeUpdateState(ConsoleEvent &event);
	void nodeUpdateAttachedCollAddress(ConsoleEvent &event);

	void nodeUpdateLog(ConsoleEvent &event);
	void nodeUpdateFileList(ConsoleEvent &event);
	void nodeUpdateExecList(ConsoleEvent &event);
};


#endif //__Console_H_
