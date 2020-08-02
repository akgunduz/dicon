//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __UnixSocket_H_
#define __UnixSocket_H_

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"

#define MAX_SIMUL_CLIENTS 10
#define UNIXSOCKETADDRESS_MASK 0xFFFFFFFFFFFFFF

class UnixSocket : public Interface {
private :
	int unixSocket;

	bool initUnixSocket();
	void runReceiver() override;
	static void runAcceptor(Interface*, int);
	void runSender(ComponentUnit& , Message *) override;
    void runMulticastSender(ComponentUnit&, Message *) override;

    static sockaddr_un getUnixAddress(Address&);

    static size_t readCB(ComponentUnit&, uint8_t*, size_t);
    static size_t writeCB(ComponentUnit&, const uint8_t*, size_t);

public :

	UnixSocket(Device*, const InterfaceSchedulerCB*, const InterfaceHostCB*);
	~UnixSocket() override;

    INTERFACE getType() override;
	bool isSupportMulticast() override;
    TypeAddressList getAddressList() override;

 //   static std::string getAddressString(Address&);

    static TypeReadCB getReadCB(ComponentUnit&);
    static TypeWriteCB getWriteCB(ComponentUnit&);
};

#endif //__UnixSocket_H_
