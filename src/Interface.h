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

#define MAX_SIMUL_CLIENTS 100
#define DEFAULT_WEB_PORT 8081

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Interface {

    const TypeHostUnit host;
    const TypeDevice device;

    Address address{};
    Address multicastAddress{};

    const InterfaceSchedulerCB *schedulerCB{};

protected :

	Scheduler *scheduler;
	std::thread threadRcv;
	int notifierPipe[2]{};

	virtual bool runReceiver() = 0;
	virtual bool runSender(ComponentUnit, TypeMessage) = 0;
	virtual bool runMulticastSender(ComponentUnit, TypeMessage) = 0;

	bool initThread();
	void end();
	Interface(const TypeHostUnit&, const TypeDevice&, const InterfaceSchedulerCB *);

public :

    virtual ~Interface();

	bool push(MSG_DIR, CommUnit&, TypeMessage);
    const TypeHostUnit& getHost() const;
    const TypeDevice& getDevice() const;
    Address& getAddress();
    Address& getMulticastAddress();

    void setAddress(Address&);
    void setMulticastAddress(Address&);

    virtual INTERFACE getType() = 0;
    virtual bool isSupportMulticast() = 0;
    virtual TypeAddressList getAddressList() = 0;
};

typedef std::shared_ptr<Interface> TypeInterface;
typedef std::map<COMPONENT, TypeInterface> TypeInterfaceList;

#endif //DICON_INTERFACE_H
