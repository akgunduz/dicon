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
	int mDesc[2];
	bool init(uint32_t);
	void runReceiver();
	void runSender(uint64_t, Message *);
	void setAddress(uint32_t);
	INTERFACES getType();
public :
	Pipe(uint32_t, const CallBack *, const std::string &);
	std::vector<uint64_t> getAddressList();
	~Pipe();
};

#endif //__Pipe_H_
