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

#define DEFAULT_IPRANGE 100
#define MAX_SIMUL_CLIENTS 10

class Net : public Interface {
private :
	int netSocket;
	int multicastSocket;
	bool initTCP();
	bool initLoopBack();
	bool initMulticast();
	void runReceiver(Unit host);
	static void *runAccepter(void *);
	void runSender(long, Message *);
	void runMulticastSender(Message *);
	void setAddress(int);
	INTERFACES getType();

public :
    static std::vector<Device>interfaceList;
	Net(Unit, CONNECTTYPE, const InterfaceCallback *, const char *);
    static std::vector<Device> getInterfaces();
	std::vector<long> getAddressList();
	~Net();
};

#endif //__Server_H_
