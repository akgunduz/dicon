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
	void runReceiver();
	static void runAccepter(Interface *, int);
	void runSender(long, Message *);
    void runMulticastSender(Message *);

    static sockaddr_un getUnixAddress(long);

public :

	UnixSocket(Device*, const InterfaceSchedulerCB*, const InterfaceHostCB*);
	~UnixSocket();

    INTERFACE getType();
	bool isSupportMulticast();

    static std::string getAddressString(long);
	static std::vector<long> getAddressList(Device*);
};

#endif //__UnixSocket_H_
