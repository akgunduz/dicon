//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_SCHEDULER_H
#define	DICON_SCHEDULER_H

#include "Common.h"
#include "Message.h"
#include "Address.h"
#include "InterfaceCallback.h"
#include "MessageTypes.h"
#include "SchedulerItem.h"

#define MAX_SCHEDULER_CAPACITY 10000

class Scheduler {

    bool ownThread;

    std::mutex mutex;

    std::condition_variable cond;

    std::thread thread;

	std::list<SchedulerItem*> items;

	std::map<int, const InterfaceSchedulerCB*> callbacks;

	int capacity;

	bool initialized;

	static void* run(void *);

public:

	Scheduler(bool seperateThread = true);

    void setCB(int, const InterfaceSchedulerCB*);

	bool push(SchedulerItem *item);

    SchedulerItem* pull();

	void end();

    virtual ~Scheduler();

};

#endif //DICON_SCHEDULER_H
