//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef __Interface_H_
#define __Interface_H_

#include <sys/un.h>
#include "Scheduler.h"
#include "Message.h"
#include "Address.h"

class Interface;

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Argument {

public:
	union {
		int acceptSocket;
		Message *msg;
	} var;

	Address *address;

	Interface *interface;

	Argument(Interface *c) : interface(c){

	}
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
	static bool senderCB(void *, Address*, Message *);
protected :
	bool initialized = false;
	Address *address;
	Scheduler *scheduler;
	pthread_t thread;
	int notifierPipe[2];
	virtual bool init(uint32_t) = 0;
	virtual void runReceiver() = 0;
	virtual void runSender(Address*, Message *) = 0;

	bool initThread();
	void end();
	Interface(INTERFACES type, const InterfaceCallback *, const std::string &);
	virtual void setAddress(uint32_t) = 0;

public :
	std::string rootPath;
	bool push(MESSAGE_DIRECTION, Address*, Message *);
	int getNotifier(NOTIFIER_TYPE type);
	virtual INTERFACES getType() = 0;
	virtual Address* getAddress();
	virtual std::vector<long> getAddressList() = 0;
	virtual ~Interface();
};


#endif //__Connector_H_
