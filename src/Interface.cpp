//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"

Interface::Interface(Device *device, const InterfaceSchedulerCB *receiveCB, const InterfaceHostCB *hostCB)
                    : hostCB(hostCB){

    scheduler = new Scheduler();

    this->address = 0;
    this->multicastAddress = 0;

    setDevice(device);

    schedulerCB = new InterfaceSchedulerCB(runSenderCB, this);

	scheduler->setCB(MESSAGE_RECEIVE, receiveCB);
	scheduler->setCB(MESSAGE_SEND, schedulerCB);
}

void Interface::end() {

	char buf[1] = {SHUTDOWN_NOTIFIER};

	write(notifierPipe[1], buf, 1);

	threadRcv.join();

}

bool Interface::initThread() {

	if (pipe(notifierPipe) < 0) {
		LOGS_E(getHost(), "Notifier Pipe Init failed with err : %d!!!", errno);
		return false;
	}

	LOGS_T(getHost(), "Init Notifier PIPE OK!!!");

	threadRcv = std::thread(runReceiverCB, this);

	return true;
}

void Interface::runReceiverCB(Interface *interface) {

    interface->runReceiver();

}

bool Interface::runSenderCB(void *arg, SchedulerItem *item) {

	auto *interface = (Interface *) arg;
    auto *msgItem = (MessageItem*) item;

	if (msgItem->address != interface->getMulticastAddress()) {

		interface->runSender(msgItem->address, msgItem->msg);

	} else {

		interface->runMulticastSender(msgItem->msg);
	}

	return true;
}

Interface::~Interface() {

    delete schedulerCB;

	close(notifierPipe[1]);
	close(notifierPipe[0]);
}

bool Interface::push(MESSAGE_DIRECTION type, long target, Message *msg) {

	if (AddressHelper::getInterface(target) == getType()) {

		scheduler->push(new MessageItem(type, target, msg));
		return true;
	}

	LOGS_E(getHost(), "Interface is not suitable for target : %d", target);
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

ComponentObject Interface::getHost() {

    return hostCB->hcb(hostCB->arg);
}

Device *Interface::getDevice() {

    return device;
}

void Interface::setDevice(Device *device) {

    this->device = device;
}
