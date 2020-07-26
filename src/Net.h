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
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001

class Net : public Interface {

private :
	int netSocket;
	int multicastSocket;

	bool initTCP();
	bool initMulticast();
	void runReceiver();
	static void runAccepter(Interface *, long);
	void runSender(long, Message *);
	void runMulticastSender(Message *);

    static std::string getIPString(long);
    static long parseIPAddress(const std::string&);

    static sockaddr_in getInetAddressByAddress(long);
    static sockaddr_in getInetAddressByPort(int);
    static ip_mreq getInetMulticastAddress(long);

    static size_t readCB(long, uint8_t*, size_t);
    static size_t readMulticastCB(long, uint8_t*, size_t);
    static size_t writeCB(long, const uint8_t*, size_t);
    static size_t writeMulticastCB(long, const uint8_t*, size_t);
public :

	Net(Device*, const InterfaceSchedulerCB*, const InterfaceHostCB*);
	~Net();

    INTERFACE getType();
	bool isSupportMulticast();

    static std::string getAddressString(long);
	static std::vector<long> getAddressList(Device*);

	static long parseAddress(std::string);

    static TypeReadCB getReadCB(long);
    static TypeWriteCB getWriteCB(long);
};

#endif //__Server_H_
