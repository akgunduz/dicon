//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Scheduler.h"

Scheduler::Scheduler(bool ownThread) {

    this->initialized = false;
    this->ownThread = ownThread;
	this->capacity = MAX_SCHEDULER_CAPACITY;

    if (ownThread) {

        thread = std::thread(run, this);

    }

	initialized = true;
}

Scheduler::~Scheduler() {

    end();

}

bool Scheduler::push(SchedulerItem *item) {

    std::unique_lock<std::mutex> lock(mutex);

	if (items.size() < capacity) {

		items.push_back(item);
        lock.unlock();
        if (ownThread) {
            cond.notify_one();
        }
		return true;
	}

    lock.unlock();

	return false;
}

SchedulerItem* Scheduler::pull() {

    std::unique_lock<std::mutex> lock(mutex);

    if (items.empty()) {

        if (ownThread) {
            cond.wait(lock);

        } else {
            lock.unlock();
            return nullptr;
        }
    }

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
    assert(item != nullptr);
    items.erase(ref);

    lock.unlock();

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

    if (ownThread) {
        push(new SchedulerItem());
        thread.join();
    }

    items.clear();
}

void Scheduler::setCB(int id, const InterfaceSchedulerCB *cb) {

	callbacks[id] = cb;
}
