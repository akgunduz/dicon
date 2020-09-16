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
    bool onSend(const TypeComponentUnit&, TypeMessage) override;

protected:

    bool initInterface() override;

public :

	CommUnixSocket(const TypeHostUnit&, const TypeDevice&, const CommInterfaceCB *);
	~CommUnixSocket() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;
    TypeAddressList getAddressList() override;

};

#endif //DICON_COMMUNIXSOCKET_H
