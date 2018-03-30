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

#define DEFAULT_IPRANGE 100
#define MAX_SIMUL_CLIENTS 10
#define LOOPBACK_RANGE 256
#define LOOPBACK_ADDRESS 0x7F000001
#define MULTICAST_ADDRESS 0xE9010101
#define IPADDRESS_MASK 0xFFFFFFFF
#define PORT_MASK 0xFFFF
#define NETMASK_MASK 0xFF
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001

class Net : public Interface {

private :
	int netSocket;
	int multicastSocket;

	bool initTCP();
	bool initMulticast();
	void runReceiver(Unit host);
	static void *runAccepter(void *);
	void runSender(long, Message *);
	void runMulticastSender(Message *);

    static int address2prefix(long);
    static std::string getIPString(long);
	static sockaddr_in getInetAddressByAddress(long);
	static sockaddr_in getInetAddressByPort(int);
    static ip_mreq getInetMulticastAddress(long);

    static std::vector<long> getAddressList(Device*);
	static std::vector<Device*>deviceList;

public :

	Net(Unit, Device*, const InterfaceCallback *);
	~Net();

    INTERFACES getType();
	bool isSupportMulticast();

	static bool createDevices();
	static std::vector<Device*>* getDevices();
    static std::string getAddressString(long);

};

#endif //__Server_H_
