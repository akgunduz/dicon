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
	int mSocket;
	bool init(uint32_t);
	void runReceiver();
	static void *runAccepter(void *);
	void runSender(uint64_t, Message *);
	void setAddress(uint32_t);
	INTERFACES getType();

public :
	static uint16_t gOffset;
	UnixSocket(uint32_t, const CallBack *, const std::string &);
	std::vector<uint64_t> getAddressList();
	~UnixSocket();
};

#endif //__UnixSocket_H_
