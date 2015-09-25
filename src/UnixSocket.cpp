//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdexcept>
#include <ifaddrs.h>
#include <sys/stat.h>
#include <dirent.h>
#include "UnixSocket.h"
#include "Tools.h"

uint16_t UnixSocket::gOffset = 1001;

UnixSocket::UnixSocket(uint32_t interfaceIndex, const CallBack *cb, const std::string &rootPath)
		: Interface(INTERFACE_UNIXSOCKET, cb, rootPath) {

	if (!init(interfaceIndex)) {
		LOG_E("Instance create failed!!!");
		throw std::runtime_error("NetReceiver : Instance create failed!!!");
	}
	LOG_I("Instance is created, Socket : %d!!!", mSocket);

}

bool UnixSocket::init(uint32_t interfaceIndex) {
	mSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (mSocket < 0) {
		LOG_E("Socket receiver open with err : %d!!!", errno);
		return false;
	}

	setAddress(interfaceIndex);

	struct sockaddr_un serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	sprintf(serv_addr.sun_path, "%s%s%d%s", UNIXSOCKET_PATH, UNIXSOCKET_FILE_PREFIX,
			(uint32_t)mAddress, UNIXSOCKET_FILE_SUFFIX);
	unlink(serv_addr.sun_path);

	socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(serv_addr.sun_path);

	int on = 1;
	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
		LOG_E("Socket option with err : %d!!!", errno);
		close(mSocket);
		return false;
	}
	LOG_T("Socket set option is OK!!!");

	if (bind(mSocket, (struct sockaddr *) &serv_addr, len) < 0) {
		LOG_E("Socket bind with err : %d!!!", errno);
		close(mSocket);
		return false;
	}
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

void UnixSocket::runReceiver() {

	bool thread_started = true;

	int maxfd = std::max(mSocket, mNotifierPipe[0]) + 1;

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

			int acceptfd = accept(mSocket, nullptr, nullptr);
			if (acceptfd < 0) {
				LOG_E("Client Socket open with err : %d!!!", errno);
				return;
			}

			struct Argument *argument = new struct Argument(this);
			argument->var.acceptSocket = acceptfd;

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

void *UnixSocket::runAccepter(void *arg) {

	struct Argument *argument = (struct Argument *) arg;

	Message *msg = new Message(argument->interface->mRootPath);
	if (msg->readFromStream(argument->var.acceptSocket)) {
		argument->interface->push(MESSAGE_RECEIVE, msg->getOwnerAddress(), msg);
	}

	delete argument;
	return nullptr;
}

void UnixSocket::runSender(uint64_t target, Message *msg) {

	int clisocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clisocket < 0) {
		LOG_E("Socket sender open with err : %d!!!", errno);
		return;
	}
	LOG_T("Socket sender %d is opened !!!", clisocket);

	struct sockaddr_un cli_addr;
	bzero((char *) &cli_addr, sizeof(cli_addr));

	cli_addr.sun_family = AF_UNIX;

	sprintf(cli_addr.sun_path, "%s%s%d%s", UNIXSOCKET_PATH, UNIXSOCKET_FILE_PREFIX,
			(uint32_t)target, UNIXSOCKET_FILE_SUFFIX);

	socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(cli_addr.sun_path);

	if (connect(clisocket, (struct sockaddr *)&cli_addr, len) == -1) {
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

UnixSocket::~UnixSocket() {
	end();
	close(mSocket);
}


void UnixSocket::setAddress(uint32_t index) {
	mAddress = (((unsigned)getpid() << 10) & 0xFFFFFF) |  gOffset++;
}

INTERFACES UnixSocket::getType() {

	return INTERFACE_UNIXSOCKET;

}

std::vector<uint64_t> UnixSocket::getAddressList() {

	std::vector<uint64_t> list;

	if (mAddress == 0) {
		return list;
	}

	DIR *unixdir = opendir(UNIXSOCKET_PATH);
	if (!unixdir) {
		printf("Can not open unix socket path!!!");
		return list;
	}

	std::string path1 = "";

	path1 = UNIXSOCKET_FILE_PREFIX;
	path1.append(Tools::getAddressStr(mAddress));
	path1.append(UNIXSOCKET_FILE_SUFFIX);

	dirent *entry;

	while ((entry = readdir(unixdir)) != NULL) {

		if (strncmp(entry->d_name, UNIXSOCKET_FILE_PREFIX, strlen(UNIXSOCKET_FILE_PREFIX)) != 0) {
			continue;
		}

		if (path1.compare(entry->d_name) == 0) {
			continue;
		}

		std::string path = entry->d_name;

		uint32_t start = (uint32_t)path.find('_') + 1;
		std::string saddress = path.substr(start, path.find('.') - start);
		uint64_t address = (unsigned)atol(saddress.c_str());

		list.push_back(address);

	}

	closedir(unixdir);

	return list;
}
