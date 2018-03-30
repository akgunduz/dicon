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
	void runReceiver(Unit host);
	static void *runAccepter(void *);
	void runSender(long, Message *);
    void runMulticastSender(Message *);

    static sockaddr_un getUnixAddress(long);

    static std::vector<long> getAddressList(Device*);
	static std::vector<Device*>deviceList;

public :

	UnixSocket(Unit, Device*, const InterfaceCallback *);
	~UnixSocket();

    INTERFACES getType();
	bool isSupportMulticast();


	static bool createDevices();
    static std::vector<Device*>* getDevices();
    static std::string getAddressString(long);
};

#endif //__UnixSocket_H_
