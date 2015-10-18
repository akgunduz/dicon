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
	bool init(int);
	void runReceiver(Unit host);
	static void *runAccepter(void *);
	void runSender(long, Message *);
	void setAddress(int);
	INTERFACES getType();

public :
    static std::vector<ConnectInterface>interfaceList;
	static uint16_t gOffset;
	Net(Unit, int, const InterfaceCallback *, const char *);
    static std::vector<ConnectInterface> getInterfaces();
	std::vector<long> getAddressList();
	~Net();
};

#endif //__Server_H_
