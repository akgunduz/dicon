//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Pipe.h"

Pipe::Pipe(uint32_t interfaceIndex, const CallBack *cb, const std::string &rootPath)
		: Interface(INTERFACE_PIPE, cb, rootPath) {

	if (!init(interfaceIndex)) {
		LOG_E("Instance create failed!!!");
		throw std::runtime_error("Pipe : Instance create failed!!!");
	}

	LOG_I("Instance is created Read : %d Write : %d !!!", mDesc[0], mDesc[1]);

}

bool Pipe::init(uint32_t interfaceIndex) {

	if (pipe(mDesc) < 0) {
		LOG_E("Init failed with err : %d!!!", errno);
		return false;
	}
	LOG_T("Pipe receiver %d is opened !!!", mDesc[1]);

	setAddress(interfaceIndex);

	if (fcntl(mDesc[0], F_SETFL, O_NONBLOCK) == -1) {
		LOG_E("Problem with Pipe States with err : %d!!!", errno);
		close(mDesc[1]);
		close(mDesc[0]);
	}


	if (!initThread()) {
		LOG_E("Problem with Pipe thread");
		close(mDesc[1]);
		close(mDesc[0]);
		return false;
	}

	return true;

}

void Pipe::runReceiver() {

	bool thread_started = true;
	int maxfd = std::max(mDesc[0], mNotifierPipe[0]) + 1;
	fd_set readfs, orjreadfs;
	FD_ZERO(&orjreadfs);
	FD_SET(mDesc[0], &orjreadfs);
	FD_SET(mNotifierPipe[0], &orjreadfs);
	while(thread_started) {
		readfs = orjreadfs;
		int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
		if (nready == -1) {
			LOG_E("Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(mDesc[0], &readfs)) {
			LOG_T("New data arrived from Pipe : %d !!!", mDesc[0]);

			Message *msg = new Message(mRootPath);
			if (msg->readFromStream(mDesc[0])) {
				push(MESSAGE_RECEIVE, msg->getOwnerAddress(), msg);
			}
		}

		if (FD_ISSET(mNotifierPipe[0], &readfs)) {
			LOG_T("New data arrived from Notifier!!!");
			char data;
			read(mNotifierPipe[0], &data, 1);
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

void Pipe::runSender(uint64_t target, Message *msg) {

	if (target <= 0) {
		return;
	}

	msg->setOwnerAddress((uint64_t)mDesc[1]);
	msg->writeToStream((int)target);

}

void Pipe::setAddress(uint32_t index) {

	mAddress = (uint64_t)mDesc[1];

}

Pipe::~Pipe() {

	end();
	close(mDesc[1]);
	close(mDesc[0]);

}

INTERFACES Pipe::getType() {

	return INTERFACE_PIPE;

}

std::vector<uint64_t> Pipe::getAddressList() {

	std::vector<uint64_t> list;

	return list;

}
