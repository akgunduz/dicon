//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Scheduler.h"

Scheduler::Scheduler() {

	thread = std::thread(run, this);

	initialized = true;
}

Scheduler::~Scheduler() {

    LOGP_T("Deallocating Scheduler");

    end();

}

bool Scheduler::push(TypeSchedulerItem item) {

    assert(item != nullptr);

    std::unique_lock<std::mutex> lock(mutex);

    if (endFlag) {

        return false;
    }

    if (items.size() >= capacity) {

        LOGP_E("Scheduler at full capacity");

        return false;
    }

    if (item->type == END_ITEM) {

        endFlag = true;
    }

    items.push_back(std::move(item));

    cond.notify_one();

    return true;
}

TypeSchedulerItem Scheduler::pull() {

    std::unique_lock<std::mutex> lock(mutex);

    cond.wait(lock, [this] {
        return !items.empty();
    });

    assert(!items.empty());

    std::list<TypeSchedulerItem>::iterator itr, ref = items.begin();

    for (itr = ref;	itr != items.end(); itr++) {

        if ((*itr)->priority < (*ref)->priority) {
            ref = itr;
        }

        if ((*itr)->priority > 0) {
            (*itr)->priority--;
        }
    }

    TypeSchedulerItem item = std::move(*(ref));

    items.erase(ref);

    return item;
}

void *Scheduler::run(void *arg) {

	auto *scheduler = (Scheduler *) arg;

	while(true) {

        TypeSchedulerItem item = scheduler->pull();

        if (item->type == END_ITEM) {
            break;
        }

        const InterfaceSchedulerCB *iCB = scheduler->callbacks[item->type];
        if (iCB != nullptr) {
            iCB->schedulerCB(iCB->arg, std::move(item));
        }
	}

	return nullptr;
}

void Scheduler::end() {

    if (!initialized) {
        return;
    }

    push(std::make_shared<SchedulerItem>());
    thread.join();

    items.clear();
}

void Scheduler::setCB(int id, const InterfaceSchedulerCB *cb) {

	callbacks[id] = cb;
}
