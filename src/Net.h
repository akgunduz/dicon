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

#define DEFAULT_PORT 61001
#define DEFAULT_IPRANGE 100
#define MAX_SIMUL_CLIENTS 10

class Net : public Interface {
private :
	int netSocket;
	bool init(uint32_t);
	void runReceiver();
	static void *runAccepter(void *);
	void runSender(Address*, Message *);
	void setAddress(uint32_t);
	INTERFACES getType();

public :
	static uint16_t gOffset;
	Net(uint32_t, const InterfaceCallback *, const std::string &);
	std::vector<long> getAddressList();
	~Net();
};

#endif //__Server_H_
