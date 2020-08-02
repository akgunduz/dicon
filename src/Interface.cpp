//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"

Interface::Interface(Device *_device, const InterfaceSchedulerCB *receiveCB, const InterfaceHostCB *hostCB)
    : device(_device), hostCB(hostCB) {

    scheduler = new Scheduler();

    schedulerCB = new InterfaceSchedulerCB(runSenderCB, this);

	scheduler->setCB(MSGDIR_RECEIVE, receiveCB);
	scheduler->setCB(MSGDIR_SEND, schedulerCB);
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

    LOGC_T(interface->getHost(), msgItem->getUnit(), MSGDIR_SEND,
           "\"%s\" is sending",
           MessageTypes::getMsgName(msgItem->getMessage()->getHeader().getType()));

	if (msgItem->getUnit().getAddress() != interface->getMulticastAddress()) {

		interface->runSender(msgItem->getUnit(), msgItem->getMessage());

	} else {

		interface->runMulticastSender(msgItem->getUnit(), msgItem->getMessage());
	}

    LOGC_D(interface->getHost(), msgItem->getUnit(), MSGDIR_SEND,
           "\"%s\" is sent",
           MessageTypes::getMsgName(msgItem->getMessage()->getHeader().getType()));

	return true;
}

Interface::~Interface() {

    delete schedulerCB;

	close(notifierPipe[1]);
	close(notifierPipe[0]);
}

bool Interface::push(MSG_DIR type, ComponentUnit& target, Message *msg) {

	if (target.getAddress().getInterface() == getType()) {

		scheduler->push(new MessageItem(type, target, msg));
		return true;
	}

	LOGS_E(getHost(), "Interface is not suitable for target : %d", target.getAddress().get().base);
	return false;
}

Address& Interface::getAddress() {

	return address;
}

Address& Interface::getMulticastAddress() {

    return multicastAddress;
}

void Interface::setAddress(Address& _address) {

    address = _address;
}

void Interface::setMulticastAddress(Address& _multicastAddress) {

    multicastAddress = _multicastAddress;
}

HostUnit& Interface::getHost() {

    return hostCB->hcb(hostCB->arg);
}

Device *Interface::getDevice() {

    return device;
}
