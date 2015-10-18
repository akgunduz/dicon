//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "Common.h"
#include "Message.h"
#include "Address.h"
#include "InterfaceCallback.h"
#include "MessageDirection.h"

#define MAX_SCHEDULER_CAPACITY 100

#define MAX_INTERFACE 10

class Capsule {

public:

	MESSAGE_DIRECTION type;
	long address;
	Message *msg;

	Capsule(MESSAGE_DIRECTION type, long address, Message *msg) {
		this->type = type;
		this->address = address;
		this->msg = msg;
	}
};

class Scheduler {
private:
	pthread_mutex_t mMutex;
	pthread_cond_t mCond;
	pthread_t mThread;
	std::list<Capsule> mMessages;
	int mCapacity;
	bool mInitialized = false;
	const InterfaceCallback* mCB[MAX_INTERFACE];
	static void* run(void *);
public:
	Scheduler(int);
	void setReceiveCB(const InterfaceCallback *);
	void setSendCB(uint16_t, const InterfaceCallback *);
	bool push(MESSAGE_DIRECTION type, long, Message *data);
	void end();
    virtual ~Scheduler();
private:

};

#endif	/* SCHEDULER_H */
