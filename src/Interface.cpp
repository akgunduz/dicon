//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"

Interface::Interface(COMPONENT host, Device *device, const InterfaceCallback *receiveCB) {

    scheduler = new Scheduler();

    this->address = 0;
    this->multicastAddress = 0;

    setHost(host);
    setDevice(device);

	scheduler->setCB(MESSAGE_RECEIVE, receiveCB);
	scheduler->setCB(MESSAGE_SEND, new InterfaceCallback(runSenderCB, this));
}

void Interface::end() {

	char buf[1] = {SHUTDOWN_NOTIFIER};

	write(notifierPipe[1], buf, 1);

	threadRcv.join();

}

bool Interface::initThread() {

	if (pipe(notifierPipe) < 0) {
		LOG_E("Notifier Pipe Init failed with err : %d!!!", errno);
		return false;
	}

	LOG_T("Init Notifier PIPE OK!!!");

	threadRcv = std::thread(runReceiverCB, this);

	return true;
}

void Interface::runReceiverCB(Interface *interface) {

    interface->runReceiver(interface->getHost());

}

bool Interface::runSenderCB(void *arg, SchedulerItem *item) {

	Interface *interface = (Interface *) arg;
	MessageItem *msgItem = (MessageItem*) item;

	if (msgItem->address != interface->getMulticastAddress()) {

		interface->runSender(msgItem->address, msgItem->msg);

	} else {

		interface->runMulticastSender(msgItem->msg);
	}

	return true;
}

Interface::~Interface() {

	close(notifierPipe[1]);
	close(notifierPipe[0]);
}

bool Interface::push(MESSAGE_DIRECTION type, long target, Message *msg) {

	if (AddressHelper::getInterface(target) == getType()) {

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

void Interface::setAddress(long address) {
    this->address = address;
}

void Interface::setMulticastAddress(long multicastAddress) {
    this->multicastAddress = multicastAddress;
}

COMPONENT Interface::getHost() {
    return host;
}

void Interface::setHost(COMPONENT host) {
    this->host = host;
}

Device *Interface::getDevice() {
    return device;
}

void Interface::setDevice(Device *device) {
    this->device = device;
}
