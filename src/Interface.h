//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef __Interface_H_
#define __Interface_H_

#include <sys/un.h>
#include "Scheduler.h"
#include "Message.h"

class Interface;

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

struct Argument {
	union {
		int acceptSocket;
		Message *msg;
	} var;
	uint64_t address;
	Interface *interface;
	Argument(Interface *c) : interface(c){}
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
	static bool senderCB(void *, uint64_t, Message *);
protected :
	bool mInitialized = false;
	uint64_t mAddress;
	Scheduler *mScheduler;
	pthread_t mThread;
	int mNotifierPipe[2];
	virtual bool init(uint32_t) = 0;
	virtual void runReceiver() = 0;
	virtual void runSender(uint64_t, Message *) = 0;

	bool initThread();
	void end();
	Interface(INTERFACES type, const CallBack *, const std::string &);
	virtual void setAddress(uint32_t) = 0;

public :
	std::string mRootPath;
	bool push(int, uint64_t, Message *);
	int getNotifier(NOTIFIER_TYPE type);
	virtual INTERFACES getType() = 0;
	virtual uint64_t getAddress();
	virtual std::vector<uint64_t> getAddressList() = 0;
	virtual ~Interface();
};


#endif //__Connector_H_
