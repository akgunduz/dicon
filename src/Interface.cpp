//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"

Interface::Interface(INTERFACES type, const InterfaceCallback *callBack, const char *rootPath) {

	try {

		scheduler = new Scheduler(MAX_SCHEDULER_CAPACITY);

	} catch (const std::runtime_error e) {
		LOG_E("Scheduler Init failed!!!");
		throw std::runtime_error("Interface : Scheduler Start failed!!!");
	}

	InterfaceCallback *interfaceCallback = new InterfaceCallback(senderCB, this);

	scheduler->setReceiveCB(callBack);
	scheduler->setSendCB(type, interfaceCallback);

	strcpy(this->rootPath, rootPath);
}

void Interface::end() {

	char buf[1] = {SHUTDOWN_NOTIFIER};

	write(notifierPipe[1], buf, 1);

	pthread_join(thread, nullptr);

}

bool Interface::initThread() {

	if (pipe(notifierPipe) < 0) {
		LOG_E("Notifier Pipe Init failed with err : %d!!!", errno);
		return false;
	}

	LOG_T("Init Notifier PIPE OK!!!");

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int pthr = pthread_create(&thread, &attr, runReceiver, (void *)this);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run thread");
		close(notifierPipe[1]);
		close(notifierPipe[0]);
		return false;
	}

	initialized = true;
	return true;
}

void *Interface::runReceiver(void *arg) {

	Interface *_interface = (Interface *)arg;
	_interface->runReceiver();
	return nullptr;

}

bool Interface::senderCB(void *arg, long address, Message *msg) {

	Argument *argument = new Argument((Interface*)arg);
	argument->msg = msg;
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

	Argument *argument = (Argument *) arg;
	argument->_interface->runSender(argument->address, argument->msg);
	delete argument;
	return nullptr;

}

Interface::~Interface() {

	close(notifierPipe[1]);
	close(notifierPipe[0]);

}

bool Interface::push(MESSAGE_DIRECTION type, long target, Message *msg) {

	if (Address::getInterface(target) == getType()) {

		scheduler->push(type, target, msg);
		return true;
	}

	LOG_E("Interface is not suitable for target : %d", target);
	return false;

}

int Interface::getNotifier(NOTIFIER_TYPE type) {

	return notifierPipe[type];
}

long Interface::getAddress() {

	if (!initialized) {
		return 0;
	}

	return address;
}
