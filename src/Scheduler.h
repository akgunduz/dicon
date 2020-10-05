//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_SCHEDULER_H
#define	DICON_SCHEDULER_H

#include <mutex>
#include <condition_variable>
#include <map>
#include <list>
#include "CommInterfaceCB.h"

#define MAX_SCHEDULER_CAPACITY 10000

class Scheduler {

    std::mutex mutex;

    std::condition_variable cond;

	std::list<TypeSchedulerItem> items;

	std::map<int, const CommInterfaceCB*> callbacks;

	size_t capacity {MAX_SCHEDULER_CAPACITY};

	bool endFlag{false};

public:

	Scheduler() = default;

    void setCB(int, const CommInterfaceCB*);

	bool push(const TypeSchedulerItem& item);

    TypeSchedulerItem pull();

    bool process();

	void shutdown();

    virtual ~Scheduler();

};

#endif //DICON_SCHEDULER_H
