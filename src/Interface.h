//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef __Interface_H_
#define __Interface_H_

#include "Scheduler.h"
#include "Message.h"
#include "Address.h"
#include "ConnectInterface.h"

class Interface;

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Argument {

public:

	int acceptSocket = 0;
	Message *msg = nullptr;

	long address = 0;

	Interface *_interface;

	Argument(Interface *c) : _interface(c){}
};

/*
class InitArgument {
public:
//	uint32_t interfaceIndex;
	std::string rootPath;
	InitArgument(uint32_t index, std::string path) : interfaceIndex(index), rootPath(path) {}
};
*/
class Interface {
private :
	static void* runReceiver(void *);
	static void* runSender(void *);
	static bool senderCB(void *, long, Message *);
protected :
	bool initialized = false;
	long address;
	Scheduler *scheduler;
	pthread_t thread;
	int notifierPipe[2];
	virtual bool init(int) = 0;
	virtual void runReceiver() = 0;
	virtual void runSender(long, Message *) = 0;

	bool initThread();
	void end();
	Interface(INTERFACES type, const InterfaceCallback *, const char *);
	virtual void setAddress(int) = 0;

public :
	char rootPath[PATH_MAX];
	bool push(MESSAGE_DIRECTION, long, Message *);
	int getNotifier(NOTIFIER_TYPE type);
	virtual INTERFACES getType() = 0;
	virtual long getAddress();
	virtual std::vector<long> getAddressList() = 0;
	virtual ~Interface();
};


#endif //__Connector_H_
