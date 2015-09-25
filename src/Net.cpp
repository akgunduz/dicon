//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdexcept>
#include <ifaddrs.h>
#include "Net.h"

uint16_t Net::gOffset = 0;


Net::Net(uint32_t interfaceIndex, const CallBack *cb, const std::string &rootPath)
		: Interface(INTERFACE_NET, cb, rootPath) {

	if (!init(interfaceIndex)) {
		LOG_E("Instance create failed!!!");
		throw std::runtime_error("NetReceiver : Instance create failed!!!");
	}
	LOG_I("Instance is created, Socket : %d!!!", mSocket);

}

bool Net::init(uint32_t interfaceIndex) {

	mSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket < 0) {
		LOG_E("Socket receiver open with err : %d!!!", errno);
		return false;
	}

	int on = 1;
	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
		LOG_E("Socket option with err : %d!!!", errno);
		close(mSocket);
		return false;
	}
	LOG_T("Socket set option is OK!!!");

	int trycount = 0;

	do {

		trycount++;

		setAddress(interfaceIndex);

		struct sockaddr_in serv_addr;
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons((uint16_t) (mAddress >> 32));

		if (bind(mSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			LOG_E("Socket bind with err : %d!!!", errno);
			if (errno != EADDRINUSE || trycount == 10) {
				close(mSocket);
				return false;
			}
			continue;
		}

		break;

	} while(1);

	LOG_T("Socket binding is OK!!!");

	if (listen(mSocket, MAX_SIMUL_CLIENTS) < 0) {
		LOG_E("Socket listen with err : %d!!!", errno);
		close(mSocket);
		return false;
	}
	LOG_T("Socket listen is OK!!!");

	if(fcntl(mSocket, F_SETFD, O_NONBLOCK) < 0) {
		LOG_E("Could not set socket Non-Blocking!!!");
		close(mSocket);
		return false;
	}
	LOG_T("Set socket Non-Blocking is OK!!!");

	if (!initThread()) {
		LOG_E("Problem with Server thread");
		close(mSocket);
		return false;
	}

	return true;
}

void Net::runReceiver() {

	bool thread_started = true;

	int maxfd = std::max(mSocket, mNotifierPipe[0]) + 1;

	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	fd_set readfs, orjreadfs;
	FD_ZERO(&orjreadfs);
	FD_SET(mSocket, &orjreadfs);
	FD_SET(mNotifierPipe[0], &orjreadfs);

	while(thread_started) {

		readfs = orjreadfs;

		int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
		if (nready == -1) {
			LOG_E("Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(mSocket, &readfs)) {

			int acceptfd = accept(mSocket, (struct sockaddr *) &cli_addr, &clilen);
			if (acceptfd < 0) {
				LOG_E("Client Socket open with err : %d!!!", errno);
				return;
			}

			struct Argument *argument = new struct Argument(this);
			argument->var.acceptSocket = acceptfd;
			argument->address = ntohl(cli_addr.sin_addr.s_addr);

			pthread_t thread;
			int pthr = pthread_create(&thread, NULL, runAccepter, (void *)argument);
			if (pthr) {
				LOG_E("Problem with runAccepter thread");
				close(acceptfd);
				thread_started = false;
			}
		}

		if (FD_ISSET(mNotifierPipe[0], &readfs)) {

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

void *Net::runAccepter(void *arg) {

	struct Argument *argument = (struct Argument *) arg;

	Message *msg = new Message(argument->interface->mRootPath);
	if (msg->readFromStream(argument->var.acceptSocket)) {
		argument->interface->push(MESSAGE_RECEIVE, msg->getOwnerAddress(), msg);
	}

	delete argument;
	return nullptr;
}

void Net::runSender(uint64_t target, Message *msg) {

	int clisocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clisocket < 0) {
		LOG_E("Socket sender open with err : %d!!!", errno);
		return;
	}
	LOG_T("Socket sender %d is opened !!!", clisocket);

	struct sockaddr_in cli_addr;
	bzero((char *) &cli_addr, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons((uint16_t)(target >> 32));
	cli_addr.sin_addr.s_addr = htonl((uint32_t)target);

	if (connect(clisocket, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) == -1) {
		LOG_E("Socket can not connect!!!");
		close(clisocket);
		return;
	}
	LOG_T("Socket sender %d is connected !!!", clisocket);

	msg->setOwnerAddress(mAddress);
	msg->writeToStream(clisocket);

	shutdown(clisocket, SHUT_RDWR);
	close(clisocket);
}



void Net::setAddress(uint32_t index) {

	mAddress = GEN_ADDRESS(ConnInterface::getNetInterfaceAddress(index), DEFAULT_PORT + gOffset++);

}

Net::~Net() {
	end();
	close(mSocket);
}


INTERFACES Net::getType() {

	return INTERFACE_NET;

}

std::vector<uint64_t> Net::getAddressList() {

	std::vector<uint64_t> list;

	if (mAddress == 0) {
		return list;
	}

	if (IS_LOOPBACK(mAddress)) {

		for (uint32_t i = 0; i < LOOPBACK_RANGE; i++) {

			uint64_t destAddress = GEN_ADDRESS(mAddress, DEFAULT_PORT + i);

			if (destAddress != mAddress) {

				list.push_back(destAddress);

			}

		}

	} else {

		uint32_t startIP = 0;

		uint32_t range = ConnInterface::getNetInterfaceInfo(mAddress, startIP);

		uint32_t ownAddress = (uint32_t)(mAddress & IPADDRESS_MASK);

		for (uint32_t i = 0; i < range; i++) {

			if (startIP != ownAddress) {

				uint64_t destAddress = GEN_ADDRESS(startIP, DEFAULT_PORT);

				list.push_back(destAddress);

			}

			startIP++;
		}

	}

	return list;
}
