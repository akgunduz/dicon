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
#include "NetAddress.h"

uint16_t Net::gOffset = 0;


Net::Net(uint32_t interfaceIndex, const InterfaceCallback *cb, const std::string &rootPath)
		: Interface(INTERFACE_NET, cb, rootPath) {

	if (!init(interfaceIndex)) {
		LOG_E("Instance create failed!!!");
		throw std::runtime_error("NetReceiver : Instance create failed!!!");
	}

	LOG_I("Instance is created, Socket : %d!!!", socket);

}

bool Net::init(uint32_t interfaceIndex) {

	netSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (netSocket < 0) {
		LOG_E("Socket receiver open with err : %d!!!", errno);
		return false;
	}

	int on = 1;
	if (setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
		LOG_E("Socket option with err : %d!!!", errno);
		close(netSocket);
		return false;
	}
	LOG_T("Socket set option is OK!!!");

    address = new NetAddress(0);

	int tryCount = 0;

	do {

        tryCount++;

		setAddress(interfaceIndex);
        struct sockaddr_in serverAddress = ((NetAddress *) address)->getInetAddress();

		if (bind(netSocket, (struct sockaddr *)&serverAddress, sizeof(sockaddr_in)) < 0) {
			LOG_E("Socket bind with err : %d!!!", errno);
			if (errno != EADDRINUSE || tryCount == 10) {
				close(netSocket);
				return false;
			}
			continue;
		}

		break;

	} while(1);

	LOG_T("Socket binding is OK!!!");

	if (listen(netSocket, MAX_SIMUL_CLIENTS) < 0) {
		LOG_E("Socket listen with err : %d!!!", errno);
		close(netSocket);
		return false;
	}
	LOG_T("Socket listen is OK!!!");

	if(fcntl(netSocket, F_SETFD, O_NONBLOCK) < 0) {
		LOG_E("Could not set socket Non-Blocking!!!");
		close(netSocket);
		return false;
	}
	LOG_T("Set socket Non-Blocking is OK!!!");

	if (!initThread()) {
		LOG_E("Problem with Server thread");
		close(netSocket);
		return false;
	}

	return true;
}

void Net::runReceiver() {

	bool thread_started = true;

	int maxfd = std::max(netSocket, notifierPipe[0]) + 1;

	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	fd_set readfs, orjreadfs;
	FD_ZERO(&orjreadfs);
	FD_SET(netSocket, &orjreadfs);
	FD_SET(notifierPipe[0], &orjreadfs);

	while(thread_started) {

		readfs = orjreadfs;

		int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
		if (nready == -1) {
			LOG_E("Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(netSocket, &readfs)) {

			int acceptfd = accept(netSocket, (struct sockaddr *) &cli_addr, &clilen);
			if (acceptfd < 0) {
				LOG_E("Client Socket open with err : %d!!!", errno);
				return;
			}

			Argument *argument = new Argument(this);
			argument->var.acceptSocket = acceptfd;
			argument->address = Address::newInstance((ntohl(cli_addr.sin_addr.s_addr)));

			pthread_t thread;
			int pthr = pthread_create(&thread, NULL, runAccepter, (void *)argument);
			if (pthr) {
				LOG_E("Problem with runAccepter thread");
				close(acceptfd);
				thread_started = false;
			}
		}

		if (FD_ISSET(notifierPipe[0], &readfs)) {

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

void *Net::runAccepter(void *arg) {

	struct Argument *argument = (struct Argument *) arg;

	Message *msg = new Message(argument->interface->rootPath);
	if (msg->readFromStream(argument->var.acceptSocket)) {
		argument->interface->push(MESSAGE_RECEIVE, new NetAddress(msg->getOwnerAddress()), msg);
	}

	delete argument;
	return nullptr;
}

void Net::runSender(Address *target, Message *msg) {

	int clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
		LOG_E("Socket sender open with err : %d!!!", errno);
		return;
	}

	LOG_T("Socket sender %d is opened !!!", clientSocket);

    sockaddr_in clientAddress = ((NetAddress *) target)->getInetAddress();

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(sockaddr_in)) == -1) {
		LOG_E("Socket can not connect!!!");
		close(clientSocket);
		return;
	}

	LOG_T("Socket sender %d is connected !!!", clientSocket);

	msg->setOwnerAddress(address->getAddress());
	msg->writeToStream(clientSocket);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}



void Net::setAddress(uint32_t index) {

	((NetAddress*)address)->set(ConnInterface::getNetInterfaceAddress(index), DEFAULT_PORT + gOffset++);

}

Net::~Net() {
	end();
	close(netSocket);
}


INTERFACES Net::getType() {

	return INTERFACE_NET;

}

std::vector<long> Net::getAddressList() {

	std::vector<long> list;

	if (((NetAddress*)address)->isLoopback()) {

		for (uint32_t i = 0; i < LOOPBACK_RANGE; i++) {

			long destAddress = GEN_ADDRESS(address->getAddress(), DEFAULT_PORT + i);

			if (destAddress != address->getAddress()) {

				list.push_back(destAddress);

			}

		}

	} else {

		uint32_t startIP = 0;

		uint32_t range = ConnInterface::getNetInterfaceInfo(address->getAddress(), startIP);

		uint32_t ownAddress = (uint32_t)(address->getAddress() & IPADDRESS_MASK);

		for (uint32_t i = 0; i < range; i++) {

			if (startIP != ownAddress) {

				long destAddress = GEN_ADDRESS(startIP, DEFAULT_PORT);

				list.push_back(destAddress);

			}

			startIP++;
		}

	}

	return list;
}
