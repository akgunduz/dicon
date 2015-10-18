//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Pipe_H_
#define __Pipe_H_

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"

class Pipe : public Interface {
private :
	int desc[2];
	bool init(int);
	void runReceiver(Unit);
	void runSender(long, Message *);
	void setAddress(int);
	INTERFACES getType();
public :
	Pipe(Unit, int, const InterfaceCallback *, const char *);
    static std::vector<ConnectInterface>interfaceList;
    static std::vector<ConnectInterface> getInterfaces();
	std::vector<long> getAddressList();
	~Pipe();
};

#endif //__Pipe_H_
