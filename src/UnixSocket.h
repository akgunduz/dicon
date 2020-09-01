//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_UNIXSOCKET_H
#define DICON_UNIXSOCKET_H

#include "Common.h"
#include "Scheduler.h"
#include "CommInterface.h"
#include "Message.h"

#define UNIXSOCKETADDRESS_MASK 0xFFFFFFFFFFFFFF

class UnixSocket : public CommInterface {

	int unixSocket;

	bool initUnixSocket();

    void onRead(ReceiveData&, ssize_t, const uv_buf_t*) override;
    bool runSender(const TypeComponentUnit& , TypeMessage) override;
    bool runMulticastSender(const TypeComponentUnit&, TypeMessage) override;

public :

	UnixSocket(const TypeHostUnit&, const TypeDevice&, const InterfaceSchedulerCB *);
	~UnixSocket() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;
    TypeAddressList getAddressList() override;

    static TypeReadCB getReadCB(const TypeComponentUnit&);
    static TypeWriteCB getWriteCB(const TypeComponentUnit&);
};

#endif //DICON_UNIXSOCKET_H
