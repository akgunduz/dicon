//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "Common.h"
#include "Message.h"

#define MAX_SCHEDULER_CAPACITY 1000

#define MAX_INTERFACE 10

enum MESSAGE_DIRECTION {
	MESSAGE_RECEIVE,
	MESSAGE_SEND
};

typedef bool (*fSchedulerCB)(void*, uint64_t, Message *);

struct Capsule {
	int type;
	uint64_t address;
	Message *msg;
};

class CallBack {
public:
	fSchedulerCB cb;
	void *arg;
	CallBack(fSchedulerCB _cb, void *_arg) : cb(_cb), arg(_arg) {}
};

class Scheduler {
private:
	pthread_mutex_t mMutex;
	pthread_cond_t mCond;
	pthread_t mThread;
	std::list<struct Capsule> mMessages;
	int mCapacity;
	bool mInitialized = false;
	const CallBack* mCB[MAX_INTERFACE];
	bool endstate = false;
	static void* run(void *);
public:
	Scheduler(int);
	void setReceiveCB(const CallBack *);
	void setSendCB(uint16_t, const CallBack *);
	bool push(int type, uint64_t, Message *data);
	bool end();
    virtual ~Scheduler();
private:

};

#endif	/* SCHEDULER_H */
