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

class UnixSocket : public Interface {
private :
	int unixSocket;
	bool init(int);
	void runReceiver();
	static void *runAccepter(void *);
	void runSender(long, Message *);
	void setAddress(int);
	INTERFACES getType();

public :
	static uint16_t gOffset;
	UnixSocket(int, const InterfaceCallback *, const char *);
	std::vector<long> getAddressList();
	~UnixSocket();
};

#endif //__UnixSocket_H_
