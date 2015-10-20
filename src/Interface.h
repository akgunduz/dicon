//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef __Interface_H_
#define __Interface_H_

#include "Scheduler.h"
#include "Message.h"
#include "Address.h"
#include "Device.h"
#include "Unit.h"

class Interface;

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Argument {

public:

	Unit host;
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
    Unit host;
    Device *device;
	long address;
    bool multicastEnabled = false;
    long multicastAddress = 0;
    char rootPath[PATH_MAX];
	Scheduler *scheduler;
	pthread_t thread;
	int notifierPipe[2];
	virtual bool init() = 0;
	virtual void runReceiver(Unit host) = 0;
	virtual void runSender(long, Message *) = 0;
	virtual void runMulticastSender(Message *) = 0;

	bool initThread();
	void end();
	Interface(Unit host, Device*, bool multicastEnabled, const InterfaceCallback *, const char *);
	virtual void setAddress(int) = 0;

public :

	bool push(MESSAGE_DIRECTION, long, Message *);
	int getNotifier(NOTIFIER_TYPE type);
    const char* getRootPath();
	virtual INTERFACES getType() = 0;
	virtual long getAddress();
    virtual Device* getDevice();
	virtual long getMulticastAddress();
	virtual bool isMulticastEnabled();
	virtual std::vector<long> getAddressList() = 0;
	virtual ~Interface();
};


#endif //__Connector_H_
