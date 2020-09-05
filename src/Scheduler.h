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
#include "MessageType.h"
#include "SchedulerItem.h"

#define MAX_SCHEDULER_CAPACITY 10000

class Scheduler {

    std::mutex mutex;

    std::condition_variable cond;

    std::thread thread;

    uv_loop_t sendLoop;

	std::list<TypeSchedulerItem> items;

	std::map<int, const InterfaceSchedulerCB*> callbacks;

	int capacity {MAX_SCHEDULER_CAPACITY};

	bool initialized{false};

	bool endFlag{false};

	static void* run(void *);

public:

	Scheduler();

    void setCB(int, const InterfaceSchedulerCB*);

    uv_loop_t* getLoop();

	bool push(TypeSchedulerItem item);

    TypeSchedulerItem pull();

	void end();

    virtual ~Scheduler();

};

#endif //DICON_SCHEDULER_H
