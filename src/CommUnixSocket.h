//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMUNIXSOCKET_H
#define DICON_COMMUNIXSOCKET_H

#include "Common.h"
#include "Scheduler.h"
#include "CommInterface.h"
#include "Message.h"

#define UNIXSOCKETADDRESS_MASK 0xFFFFFFFFFFFFFF

class CommUnixSocket : public CommInterface {

	int unixSocket{};

	bool initUnixSocket();

    bool onRead(ReceiveData&, const uint8_t *, size_t);
    bool runSender(const TypeComponentUnit& , TypeMessage) override;
    bool runMulticastSender(const TypeComponentUnit&, TypeMessage) override;

public :

	CommUnixSocket(const TypeHostUnit&, const TypeDevice&, const InterfaceSchedulerCB *);
	~CommUnixSocket() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;
    TypeAddressList getAddressList() override;
};

#endif //DICON_COMMUNIXSOCKET_H
