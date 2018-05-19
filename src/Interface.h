//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef BANKOR_INTERFACE_H
#define BANKOR_INTERFACE_H

#include "Scheduler.h"
#include "Message.h"
#include "AddressHelper.h"
#include "Device.h"
#include "MessageItem.h"

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Interface {

private :
	static void runReceiverCB(Interface *);
	static bool runSenderCB(void *, SchedulerItem *);

	Device *device;

    long address;
    long multicastAddress;

    const InterfaceSchedulerCB *schedulerCB;
    const InterfaceHostCB *hostCB;

    void setDevice(Device*);

protected :

	Scheduler *scheduler;
	std::thread threadRcv;
	int notifierPipe[2];

	virtual void runReceiver() = 0;
	virtual void runSender(long, Message *) = 0;
	virtual void runMulticastSender(Message *) = 0;

	bool initThread();
	void end();
	Interface(Device *,
              const InterfaceSchedulerCB*,
              const InterfaceHostCB*);

public :

    virtual ~Interface();

	bool push(MESSAGE_DIRECTION, long, Message *);
	ComponentObject getHost();
	Device* getDevice();
    long getAddress();
    long getMulticastAddress();

    void setAddress(long);
    void setMulticastAddress(long);

    virtual INTERFACE getType() = 0;
    virtual bool isSupportMulticast() = 0;
};


#endif //BANKOR_INTERFACE_H
