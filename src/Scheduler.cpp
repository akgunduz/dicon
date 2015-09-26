//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Scheduler.h"

Scheduler::Scheduler(int capacity) {

	mCapacity = capacity;

	int res = pthread_mutex_init(&mMutex, NULL);
	if (res) {
		LOG_E("Mutex init failed");
		throw std::runtime_error("Scheduler : Mutex init failed");
	}

	res = pthread_cond_init (&mCond, NULL);
	if (res) {
		LOG_E("Condition init fail");
		pthread_mutex_destroy(&mMutex);
		throw std::runtime_error("Scheduler : Condition init failed");
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int pthr = pthread_create(&mThread, &attr, run, (void *)this);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run thread");
		pthread_cond_destroy(&mCond);
		pthread_mutex_destroy(&mMutex);
		throw std::runtime_error("Scheduler : Problem with run thread");
	}

	mInitialized = true;
}

Scheduler::~Scheduler() {

	if (!mInitialized) {
		return;
	}

	end();

	pthread_join(mThread, nullptr);

	for (std::list<struct Capsule>::iterator itr = mMessages.begin(); itr != mMessages.end(); itr++) {
		delete itr->msg;
	}

	mMessages.clear();

	pthread_cond_destroy(&mCond);
	pthread_mutex_destroy(&mMutex);

}

bool Scheduler::push(MESSAGE_DIRECTION type, Address* target, Message *msg) {

	if (pthread_mutex_lock(&mMutex) != 0) { //replaced with trylock
		LOG_E("Can not push, system is BUSY!!!");
		return false;
	}

	if (mMessages.size() < mCapacity) {

		msg->normalizePriority();

		Capsule capsule(type, target, msg);
		mMessages.push_back(capsule);
		pthread_mutex_unlock(&mMutex);
		pthread_cond_signal(&mCond);
		return true;
	}

	pthread_mutex_unlock(&mMutex);

	return false;
}

void *Scheduler::run(void *arg) {

	Scheduler *scheduler = (Scheduler *) arg;

	while(true) {

		pthread_mutex_lock(&scheduler->mMutex);

		if (scheduler->mMessages.empty()) {
			pthread_cond_wait(&scheduler->mCond, &scheduler->mMutex);
		}

		std::list<struct Capsule>::iterator itr, ref = scheduler->mMessages.begin();

		for (itr = ref;	itr != scheduler->mMessages.end(); itr++) {

			if (itr->msg->getPriority() < ref->msg->getPriority()) {
				ref = itr;
			}

			itr->msg->iteratePriority();
		}

		struct Capsule refCapsule = *(ref);
		scheduler->mMessages.erase(ref);

		pthread_mutex_unlock(&scheduler->mMutex);

        if (refCapsule.type == MESSAGE_END) {
            break;
        }

		int interfaceType = refCapsule.type + refCapsule.address->getInterface();

		scheduler->mCB[interfaceType]->cb(scheduler->mCB[interfaceType]->arg, refCapsule.address, refCapsule.msg);
	}

	return nullptr;
}

void Scheduler::end() {

	push(MESSAGE_END, 0, nullptr);

}

void Scheduler::setReceiveCB(const InterfaceCallback *callBack) {

	mCB[MESSAGE_RECEIVE] = callBack;

}

void Scheduler::setSendCB(uint16_t interfaceID, const InterfaceCallback *callBack) {

	mCB[MESSAGE_SEND + interfaceID] = callBack;

}
