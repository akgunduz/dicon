//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_NET_H
#define DICON_NET_H

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"
#include "Component.h"
#include "InterfaceTypes.h"

#define LOOPBACK_RANGE 256
#define MULTICAST_ADDRESS 0xE9010101
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001

class Net : public Interface {

private :
	int netSocket{};
	int multicastSocket{};

	bool initTCP();
	bool initMulticast();
	void runReceiver() override;
	static void runAcceptor(Interface*, int);
	void runSender(ComponentUnit, Message *) override;
	void runMulticastSender(ComponentUnit target, Message *) override;

    static std::string getIPString(Address&);
    static long parseIPAddress(const std::string&);

    static sockaddr_in getInetAddressByAddress(Address&);
    static sockaddr_in getInetAddressByPort(int);
    static ip_mreq getInetMulticastAddress(Address&);
    static std::string getAddressString(Address&);

    static size_t readCB(ComponentUnit&, uint8_t*, size_t);
    static size_t readMulticastCB(ComponentUnit&, uint8_t*, size_t);
    static size_t writeCB(ComponentUnit&, const uint8_t*, size_t);
    static size_t writeMulticastCB(ComponentUnit&, const uint8_t*, size_t);
public :

	Net(Device*, const InterfaceSchedulerCB*, const InterfaceHostCB*);
	~Net() override;

    INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;

    static Address parseAddress(std::string);

    static TypeReadCB getReadCB(ComponentUnit&);
    static TypeWriteCB getWriteCB(ComponentUnit&);
};

#endif //DICON_NET_H
