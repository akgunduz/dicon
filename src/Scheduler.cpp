//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Scheduler.h"

Scheduler::Scheduler(bool seperateThread) {

    this->seperateThread = seperateThread;
	this->capacity = MAX_SCHEDULER_CAPACITY;

	int res = pthread_mutex_init(&mutex, NULL);
	if (res) {
		LOG_E("Mutex init failed");
		throw std::runtime_error("Scheduler : Mutex init failed");
	}

	res = pthread_cond_init (&cond, NULL);
	if (res) {
		LOG_E("Condition init fail");
		pthread_mutex_destroy(&mutex);
		throw std::runtime_error("Scheduler : Condition init failed");
	}

    if (seperateThread) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        int pthr = pthread_create(&thread, &attr, run, (void *) this);
        pthread_attr_destroy(&attr);
        if (pthr) {
            LOG_E("Problem with run thread");
            pthread_cond_destroy(&cond);
            pthread_mutex_destroy(&mutex);
            throw std::runtime_error("Scheduler : Problem with run thread");
        }
    }

	initialized = true;
}

Scheduler::~Scheduler() {

	if (!initialized) {
		return;
	}

    end();

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);

}

bool Scheduler::push(SchedulerItem *item) {

	if (pthread_mutex_lock(&mutex) != 0) { //replaced with trylock
		LOG_E("Can not push, system is BUSY!!!");
		return false;
	}

	if (items.size() < capacity) {

		items.push_back(item);
		pthread_mutex_unlock(&mutex);
        if (seperateThread) {
            pthread_cond_signal(&cond);
        }
		return true;
	}

    LOG_E("Too many items in scheduler, escaping them!!!");
	pthread_mutex_unlock(&mutex);

	return false;
}

SchedulerItem* Scheduler::pull() {

    if (seperateThread) {
        return nullptr;
    }

    if (pthread_mutex_lock(&mutex) != 0) { //replaced with trylock
        LOG_E("Can not push, system is BUSY!!!");
        return nullptr;
    }

    if (items.empty()) {
        pthread_mutex_unlock(&mutex);
        return nullptr;
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
    items.erase(ref);

    pthread_mutex_unlock(&mutex);

    return item;
}

void *Scheduler::run(void *arg) {

	Scheduler *scheduler = (Scheduler *) arg;

	while(true) {

		pthread_mutex_lock(&scheduler->mutex);

		if (scheduler->items.empty()) {
			pthread_cond_wait(&scheduler->cond, &scheduler->mutex);
		}

		std::list<SchedulerItem*>::iterator itr, ref = scheduler->items.begin();

		for (itr = ref;	itr != scheduler->items.end(); itr++) {

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
		scheduler->items.erase(ref);

		pthread_mutex_unlock(&scheduler->mutex);

        if (item->type == END_ITEM) {
            break;
        }

        const InterfaceCallback* iCB = scheduler->callbacks[item->type];
        if (iCB != nullptr) {
            iCB->cb(iCB->arg, item);
        }
	}

	return nullptr;
}

void Scheduler::end() {

    if (seperateThread) {
        push(new SchedulerItem());
        pthread_join(thread, nullptr);
    }

    items.clear();
}

void Scheduler::setCB(int id, const InterfaceCallback *cb) {
	callbacks[id] = cb;
}
