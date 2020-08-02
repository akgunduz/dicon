//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef DICON_INTERFACE_H
#define DICON_INTERFACE_H

#include "Scheduler.h"
#include "Message.h"
#include "Address.h"
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

	Device *device{};

    Address address{};
    Address multicastAddress{};

    const InterfaceSchedulerCB *schedulerCB{};
    const InterfaceHostCB *hostCB{};

protected :

	Scheduler *scheduler;
	std::thread threadRcv;
	int notifierPipe[2];

	virtual void runReceiver() = 0;
	virtual void runSender(ComponentUnit&, Message *) = 0;
	virtual void runMulticastSender(ComponentUnit&, Message *) = 0;

	bool initThread();
	void end();
	Interface(Device *,
              const InterfaceSchedulerCB*,
              const InterfaceHostCB*);

public :

    virtual ~Interface();

	bool push(MSG_DIR, ComponentUnit&, Message *);
	HostUnit& getHost();
	Device* getDevice();
    Address& getAddress();
    Address& getMulticastAddress();

    void setAddress(Address&);
    void setMulticastAddress(Address&);

    virtual INTERFACE getType() = 0;
    virtual bool isSupportMulticast() = 0;
    virtual TypeAddressList getAddressList() = 0;
};


#endif //DICON_INTERFACE_H
