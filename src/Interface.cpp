//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"
#include "Tools.h"

Interface::Interface(INTERFACES type, const CallBack *callBack, const std::string &rootPath) {

	try {

		mScheduler = new Scheduler(MAX_SCHEDULER_CAPACITY);

	} catch (const std::runtime_error e) {
		LOG_E("Scheduler Init failed!!!");
		throw std::runtime_error("Interface : Scheduler Start failed!!!");
	}

	CallBack *interfaceCallback = new CallBack(senderCB, this);

	mScheduler->setReceiveCB(callBack);
	mScheduler->setSendCB(type, interfaceCallback);

	mRootPath = rootPath;
}

void Interface::end() {

	char buf[1] = {SHUTDOWN_NOTIFIER};
	write(mNotifierPipe[1], buf, 1);
	pthread_join(mThread, nullptr);

}

bool Interface::initThread() {
	if (pipe(mNotifierPipe) < 0) {
		LOG_E("Notifier Pipe Init failed with err : %d!!!", errno);
		return false;
	}
	LOG_T("Init Notifier PIPE OK!!!");

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int pthr = pthread_create(&mThread, &attr, runReceiver, (void *)this);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run thread");
		close(mNotifierPipe[1]);
		close(mNotifierPipe[0]);
		return false;
	}
	mInitialized = true;
	return true;
}

void *Interface::runReceiver(void *arg) {
	Interface *interface = (Interface *)arg;
	interface->runReceiver();
	return nullptr;
}

bool Interface::senderCB(void *arg, uint64_t address, Message *msg) {
	struct Argument *argument = new struct Argument((Interface*)arg);
	argument->var.msg = msg;
	argument->address = address;
	pthread_t thread;
	int pthr = pthread_create(&thread, NULL, runSender, (void *)argument);
	if (pthr) {
		LOG_E("Problem with runSender thread");
		return false;
	}
	return true;
}

void* Interface::runSender(void *arg) {
	struct Argument *argument = (struct Argument *) arg;
	argument->interface->runSender(argument->address, argument->var.msg);
	delete argument;
	return nullptr;
}

Interface::~Interface() {
	close(mNotifierPipe[1]);
	close(mNotifierPipe[0]);
}

bool Interface::push(int type, uint64_t target, Message *msg) {

	INTERFACES interface = Tools::getInterface(target);
	if (interface == getType()) {

		mScheduler->push(type, target, msg);
		return true;

	}

	LOG_E("Interface is not suitable for target : %d", target);
	return false;

}

int Interface::getNotifier(NOTIFIER_TYPE type) {
	return mNotifierPipe[type];
}

uint64_t Interface::getAddress() {

	if (!mInitialized) {
		return 0;
	}

	return mAddress;
}
