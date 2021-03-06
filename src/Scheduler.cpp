//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <cassert>
#include "Scheduler.h"
#include "CommInterfaceCB.h"
#include "Log.h"

Scheduler::~Scheduler() {

    LOGP_T("Deallocating Scheduler");
}

bool Scheduler::push(const TypeSchedulerItem& item) {

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

    items.push_back(item);

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

    TypeSchedulerItem item = *ref;

    items.erase(ref);

    return item;
}

void Scheduler::shutdown() {

    push(std::make_shared<SchedulerItem>());

}

void Scheduler::setCB(int id, const CommInterfaceCB *cb) {

	callbacks[id] = cb;
}

bool Scheduler::process() {

    TypeSchedulerItem item = pull();

    const CommInterfaceCB *commInterfaceCB = callbacks[item->type];

    if (item->type == END_ITEM || !commInterfaceCB) {

        return false;
    }

    commInterfaceCB->schedulerCB(commInterfaceCB->arg, item);

    return true;
}
