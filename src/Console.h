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
	void SetClientData(void*);
};

typedef void (Console::*fConsoleUpdater)(ConsoleEvent &event);

class Console {

	Distributor *distObject;
	Collector *collObject;
	Client *clientObject;

	fConsoleUpdater uiUpdater[UI_UPDATE_MAX];

public:
	Console();

	void updateConsoleEvent(int, void*);

	void updateLog(ConsoleEvent &event);

	bool clientInit(INTERFACES, INTERFACES);
	void clientDestroy();
	void clientRemoveDir(const char *);
	void clientRun(INTERFACES, INTERFACES);
	void clientUpdateAddresses(ConsoleEvent& event);
	void clientUpdateState(ConsoleEvent& event);
	void clientUpdateAttachedCollAddress(ConsoleEvent& event);

	void clientUpdateLog(ConsoleEvent &event);
	void clientUpdateFileList(ConsoleEvent &event);
	void clientUpdateParamList(ConsoleEvent &event);
	void clientUpdateExecList(ConsoleEvent &event);
};


#endif //__Console_H_
