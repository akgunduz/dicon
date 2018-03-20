//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"
#include "NetAddress.h"

Interface::Interface(Unit host, Device *device, const InterfaceCallback *receiveCB) {

	try {

		scheduler = new Scheduler();

	} catch (const std::runtime_error e) {
		LOG_E("Scheduler Init failed!!!");
		throw std::runtime_error("Interface : Scheduler Start failed!!!");
	}

	InterfaceCallback *sendCB = new InterfaceCallback(senderCB, this);

    this->host = host;
	this->device = device;
    this->lastFreePort = DEFAULT_PORT;
    this->address = 0;
    this->multicastAddress = 0;

	scheduler->setCB(MESSAGE_RECEIVE, receiveCB);
	scheduler->setCB(MESSAGE_SEND, sendCB);
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

	Argument *argument = new Argument(this);
	argument->host = host;
    argument->_interface = this;

	int pthr = pthread_create(&thread, &attr, runReceiver, (void *)argument);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run thread");
		close(notifierPipe[1]);
		close(notifierPipe[0]);
		return false;
	}

	return true;
}

void *Interface::runReceiver(void *arg) {

    Argument *argument = (Argument *) arg;
	argument->_interface->runReceiver(argument->host);

    delete argument;
	return nullptr;
}

bool Interface::senderCB(void *arg, SchedulerItem *item) {

	Argument *argument = new Argument((Interface*)arg);
	argument->item = (MessageItem*) item;

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

    if (argument->item->address != argument->_interface->getMulticastAddress()) {
        argument->_interface->runSender(argument->item->address, argument->item->msg);

    } else {
        argument->_interface->runMulticastSender(argument->item->msg);
    }

	delete argument;
	return nullptr;
}

Interface::~Interface() {

	close(notifierPipe[1]);
	close(notifierPipe[0]);
}

bool Interface::push(MESSAGE_DIRECTION type, long target, Message *msg) {

	if (Address::getInterface(target) == getType()) {

		scheduler->push(new MessageItem(type, target, msg));
		return true;
	}

	LOG_E("Interface is not suitable for target : %d", target);
	return false;
}

long Interface::getAddress() {
	return address;
}

long Interface::getMulticastAddress() {
    return multicastAddress;
}

Device *Interface::getDevice() {
    return device;
}