//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Scheduler.h"

Scheduler::Scheduler() {

    this->initialized = false;
	this->capacity = MAX_SCHEDULER_CAPACITY;

	thread = std::thread(run, this);

	initialized = true;
}

Scheduler::~Scheduler() {

    end();

}

bool Scheduler::push(SchedulerItem *item) {

    assert(item != nullptr);

    std::unique_lock<std::mutex> lock(mutex);

	if (items.size() < capacity) {

		items.push_back(item);

        cond.notify_one();

		return true;
	}

	return false;
}

SchedulerItem* Scheduler::pull() {

    std::unique_lock<std::mutex> lock(mutex);

    cond.wait(lock, [this] {
        return !items.empty();
    });

    assert(!items.empty());

    std::list<SchedulerItem*>::iterator itr, ref = items.begin();

    for (itr = ref;	itr != items.end(); itr++) {

        SchedulerItem *itrItem = *itr;
        SchedulerItem *refItem = *ref;
        if (itrItem->priority < refItem->priority) {
            ref = itr;
        }

        if (itrItem->priority > 0) {
            itrItem->priority--;
        }
    }

    SchedulerItem *item = *(ref);

    items.erase(ref);

    return item;
}

void *Scheduler::run(void *arg) {

	auto *scheduler = (Scheduler *) arg;

	while(true) {

        SchedulerItem *item = scheduler->pull();

        if (item->type == END_ITEM) {
            delete item;
            break;
        }

        const InterfaceSchedulerCB *iCB = scheduler->callbacks[item->type];
        if (iCB != nullptr) {
            iCB->schedulerCB(iCB->arg, item);
        }

        delete item;
	}

	return nullptr;
}

void Scheduler::end() {

    if (!initialized) {
        return;
    }

    push(new SchedulerItem());
    thread.join();

    items.clear();
}

void Scheduler::setCB(int id, const InterfaceSchedulerCB *cb) {

	callbacks[id] = cb;
}
