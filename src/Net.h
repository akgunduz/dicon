//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NET_H
#define DICON_NET_H

#include "Common.h"
#include "Scheduler.h"
#include "CommInterface.h"
#include "Message.h"
#include "Component.h"
#include "CommInterfaceType.h"

#define LOOPBACK_RANGE 256
#define MULTICAST_ADDRESS 0xE9010101
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001

class Net : public CommInterface {

	int netSocket{};
	int multicastSocket{};

	bool initTCP();
	bool initMulticast();
    bool runReceiver() override;
	bool runSender(const TypeComponentUnit&, TypeMessage) override;
    bool runMulticastSender(const TypeComponentUnit&, TypeMessage) override;

public :

	Net(const TypeHostUnit&, const TypeDevice&,
        const InterfaceSchedulerCB *);
	~Net() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;

    static TypeReadCB getReadCB(const TypeComponentUnit&);
    static TypeWriteCB getWriteCB(const TypeComponentUnit&);
};

#endif //DICON_NET_H
