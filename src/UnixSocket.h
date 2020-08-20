//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_UNIXSOCKET_H
#define DICON_UNIXSOCKET_H

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"

#define UNIXSOCKETADDRESS_MASK 0xFFFFFFFFFFFFFF

class UnixSocket : public Interface {

	int unixSocket;

	bool initUnixSocket();
    bool runReceiver() override;
    bool runSender(ComponentUnit , TypeMessage) override;
    bool runMulticastSender(ComponentUnit, TypeMessage) override;

public :

	UnixSocket(Device*, const InterfaceSchedulerCB*, const InterfaceHostCB*);
	~UnixSocket() override;

    INTERFACE getType() override;
	bool isSupportMulticast() override;
    TypeAddressList getAddressList() override;

    static TypeReadCB getReadCB(ComponentUnit&);
    static TypeWriteCB getWriteCB(ComponentUnit&);
};

#endif //DICON_UNIXSOCKET_H
