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
#include "MessageItem.h"

class Interface;

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Argument {

public:

	COMPONENT host;
	int acceptSocket;
	MessageItem *item;

	Interface *_interface;

	Argument(Interface *c, long address) : _interface(c), acceptSocket(0){
		item = new MessageItem(MESSAGE_RECEIVE, address);
	}

	Argument(Interface *c) : _interface(c), acceptSocket(0){
		item = new MessageItem(MESSAGE_RECEIVE);
	}
};

class Interface {

private :
	static void* runReceiver(void *);
	static void* runSender(void *);
	static bool senderCB(void *, SchedulerItem *);

	Device *device;
	COMPONENT host;

    long address;
    long multicastAddress;

    void setHost(COMPONENT);
    void setDevice(Device*);

protected :

	Scheduler *scheduler;
	pthread_t thread;
	int notifierPipe[2];

	virtual void runReceiver(COMPONENT host) = 0;
	virtual void runSender(long, Message *) = 0;
	virtual void runMulticastSender(Message *) = 0;

	bool initThread();
	void end();
	Interface(COMPONENT host, Device *, const InterfaceCallback *);

public :
    virtual ~Interface();

	bool push(MESSAGE_DIRECTION, long, Message *);
	COMPONENT getHost();
	Device* getDevice();
    long getAddress();
    long getMulticastAddress();

    void setAddress(long);
    void setMulticastAddress(long);

    virtual INTERFACE getType() = 0;
    virtual bool isSupportMulticast() = 0;
};


#endif //__Connector_H_
