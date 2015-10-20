//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Pipe.h"
#include "PipeAddress.h"

std::vector<Device> Pipe::interfaceList;

Pipe::Pipe(Unit host, Device* device, bool multicastEnabled, const InterfaceCallback *cb, const char *rootPath)
		: Interface(host, device, multicastEnabled, cb, rootPath) {

	if (!init()) {
		LOG_E("Instance create failed!!!");
		throw std::runtime_error("Pipe : Instance create failed!!!");
	}

	LOG_I("Instance is created Read : %d Write : %d !!!", desc[0], desc[1]);

}

bool Pipe::init() {

	if (pipe(desc) < 0) {
		LOG_E("Init failed with err : %d!!!", errno);
		return false;
	}
	LOG_T("Pipe receiver %d is opened !!!", desc[1]);

	setAddress(0);

	if (fcntl(desc[0], F_SETFL, O_NONBLOCK) == -1) {
		LOG_E("Problem with Pipe States with err : %d!!!", errno);
		close(desc[1]);
		close(desc[0]);
	}


	if (!initThread()) {
		LOG_E("Problem with Pipe thread");
		close(desc[1]);
		close(desc[0]);
		return false;
	}

	return true;

}

void Pipe::runReceiver(Unit host) {

	bool thread_started = true;
	int maxfd = std::max(desc[0], notifierPipe[0]) + 1;
	fd_set readfs, orjreadfs;
	FD_ZERO(&orjreadfs);
	FD_SET(desc[0], &orjreadfs);
	FD_SET(notifierPipe[0], &orjreadfs);
	while(thread_started) {
		readfs = orjreadfs;
		int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
		if (nready == -1) {
			LOG_E("Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(desc[0], &readfs)) {
			LOG_T("New data arrived from Pipe : %d !!!", desc[0]);

			Message *msg = new Message(host, getRootPath());
			if (msg->readFromStream(desc[0])) {
				push(MESSAGE_RECEIVE, msg->getOwnerAddress(), msg);
			}
		}

		if (FD_ISSET(notifierPipe[0], &readfs)) {
			LOG_T("New data arrived from Notifier!!!");
			char data;
			read(notifierPipe[0], &data, 1);
			switch(data) {
				case SHUTDOWN_NOTIFIER:
					thread_started = false;
					break;
				default:
					break;
			}
		}
	}
}

void Pipe::runSender(long target, Message *msg) {

	if (target <= 0) {
		return;
	}

	msg->setOwnerAddress(address);
	msg->writeToStream((int)target);

}

void Pipe::setAddress(int portIndex) {

	address = (long) desc[1];

}

Pipe::~Pipe() {

	end();
	close(desc[1]);
	close(desc[0]);

}

INTERFACES Pipe::getType() {

	return INTERFACE_PIPE;

}

std::vector<long> Pipe::getAddressList() {

	std::vector<long> list;

	return list;

}

std::vector<Device> Pipe::getInterfaces() {

    if (interfaceList.size() > 0) {
        return interfaceList;
    }

    interfaceList.push_back(Device("pp", INTERFACE_PIPE));

    return interfaceList;
}

void Pipe::runMulticastSender(Message *message) {

}
