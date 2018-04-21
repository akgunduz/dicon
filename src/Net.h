//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Net_H_
#define __Net_H_

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"
#include "Component.h"
#include "InterfaceTypes.h"

#define MAX_SIMUL_CLIENTS 10
#define LOOPBACK_RANGE 256
#define MULTICAST_ADDRESS 0xE9010101
#define PORT_MASK 0xFFFF
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001

class Net : public Interface {

private :
	int netSocket;
	int multicastSocket;

	bool initTCP();
	bool initMulticast();
	void runReceiver(COMPONENT);
	static void runAccepter(Interface *, int);
	void runSender(long, Message *);
	void runMulticastSender(Message *);

    static std::string getIPString(long);
    static long parseIPAddress(std::string);

	static sockaddr_in getInetAddressByAddress(long);
	static sockaddr_in getInetAddressByPort(int);
    static ip_mreq getInetMulticastAddress(long);

public :

	Net(COMPONENT, Device*, const InterfaceCallback *);
	~Net();

    INTERFACE getType();
	bool isSupportMulticast();

    static std::string getAddressString(long);
	static std::vector<long> getAddressList(Device*);

	static long parseAddress(std::string);

};

#endif //__Server_H_
