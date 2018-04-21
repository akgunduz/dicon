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
#include "SchedulerItem.h"

#define MAX_SCHEDULER_CAPACITY 10000

class Scheduler {
private:
    bool ownThread;
    std::mutex mutex;
    std::condition_variable cond;
    std::thread thread;
	std::list<SchedulerItem*> items;
	std::map<int, const InterfaceCallback*> callbacks;
	int capacity;
	bool initialized;
	static void* run(void *);
public:
	Scheduler(bool seperateThread = true);
    void setCB(int, const InterfaceCallback *);
	bool push(SchedulerItem *item);
    SchedulerItem* pull();
	void end();
    virtual ~Scheduler();
private:

};

#endif	/* SCHEDULER_H */
