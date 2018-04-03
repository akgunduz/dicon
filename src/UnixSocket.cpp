//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "UnixSocket.h"

UnixSocket::UnixSocket(Unit host, Device *device, const InterfaceCallback *cb)
		: Interface(host, device, cb) {

	if (!initUnixSocket()) {
		throw std::runtime_error("initUnixSocket failed!!!");
	}

    if (!initThread()) {
		throw std::runtime_error("initThread failed!!!");
    }
}

bool UnixSocket::initUnixSocket() {

    unixSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unixSocket < 0) {
        LOG_E("Socket receiver open with err : %d!!!", errno);
        return false;
    }

	int tryCount = 10;
    int lastFreePort = 0;

	for (int j = tryCount; j > 0; j--) {

        long address = Address::createAddress(getDevice()->getType(), getDevice()->getBase(), lastFreePort, getDevice()->getHelper());

		sockaddr_un serverAddress = getUnixAddress(address);

		socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t) strlen(serverAddress.sun_path);

		if (bind(unixSocket, (struct sockaddr *) &serverAddress, len) < 0) {

			lastFreePort++;
			continue;
		}

		if (listen(unixSocket, MAX_SIMUL_CLIENTS) < 0) {
			LOG_E("Socket listen with err : %d!!!", errno);
			close(unixSocket);
			return false;
		}

		if (fcntl(unixSocket, F_SETFD, O_NONBLOCK) < 0) {
			LOG_E("Could not set socket Non-Blocking!!!");
			close(unixSocket);
			return false;
		}

		setAddress(address);

        LOG_I("Using address : %s", getAddressString(address).c_str());

		return true;
	}

    LOG_E("Could not set create unix socket!!!");

    close(unixSocket);

    return false;
}

void UnixSocket::runReceiver(Unit host) {

	bool thread_started = true;

	int maxfd = std::max(unixSocket, notifierPipe[0]) + 1;

	fd_set readfs, orjreadfs;

	FD_ZERO(&orjreadfs);
	FD_SET(unixSocket, &orjreadfs);
	FD_SET(notifierPipe[0], &orjreadfs);

	while(thread_started) {

		readfs = orjreadfs;

		int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
		if (nready == -1) {
			LOG_E("Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(unixSocket, &readfs)) {

			int acceptfd = accept(unixSocket, nullptr, nullptr);
			if (acceptfd < 0) {
				LOG_E("Node Socket open with err : %d!!!", errno);
				return;
			}

			Argument *argument = new Argument(this);
			argument->host = host;
			argument->acceptSocket = acceptfd;

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

void *UnixSocket::runAccepter(void *arg) {

	Argument *argument = (Argument *) arg;

	Message *msg = new Message(argument->host);
	if (msg->readFromStream(argument->acceptSocket)) {
		argument->_interface->push(MESSAGE_RECEIVE, msg->getHeader()->getOwnerAddress(), msg);
	}

	delete argument;
	return nullptr;
}

void UnixSocket::runSender(long target, Message *msg) {

	int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clientSocket < 0) {
		LOG_E("Socket sender open with err : %d!!!", errno);
		return;
	}

	LOG_T("Socket sender %d is opened !!!", clientSocket);

    sockaddr_un clientAddress = getUnixAddress(target);

	socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(clientAddress.sun_path);

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, len) == -1) {
		LOG_E("Socket can not connect!!!");
		close(clientSocket);
		return;
	}

	LOG_T("Socket sender %d is connected !!!", clientSocket);

	msg->getHeader()->setOwnerAddress(getAddress());
	msg->writeToStream(clientSocket);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

void UnixSocket::runMulticastSender(Message *message) {

}

UnixSocket::~UnixSocket() {
	end();
	close(unixSocket);
}


INTERFACE UnixSocket::getType() {

	return INTERFACE_UNIXSOCKET;

}

bool UnixSocket::isSupportMulticast() {

    return false;
}

std::string UnixSocket::getAddressString(long address) {

    char sAddress[50];
    sprintf(sAddress, "%ld", address);
    return std::string(sAddress);
}

sockaddr_un UnixSocket::getUnixAddress(long address) {

    sockaddr_un unix_addr;
    bzero((char *) &unix_addr, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s%s%ld%s", UNIXSOCKET_PATH, UNIXSOCKET_FILE_PREFIX,
            address, UNIXSOCKET_FILE_SUFFIX);
    return unix_addr;
}

std::vector<long> UnixSocket::getAddressList(Device* device) {

    std::vector<long> list;

    DIR *unixdir = opendir(UNIXSOCKET_PATH);
    if (!unixdir) {
        printf("Can not open unix socket path!!!");
        return list;
    }

    dirent *entry;

    while ((entry = readdir(unixdir)) != NULL) {

        if (strncmp(entry->d_name, UNIXSOCKET_FILE_PREFIX, strlen(UNIXSOCKET_FILE_PREFIX)) != 0) {
            continue;
        }

        std::string path = entry->d_name;

        uint32_t start = (uint32_t)path.find('_') + 1;
        std::string saddress = path.substr(start, path.find('.') - start);
        long address = atol(saddress.c_str());

        list.push_back(address);

    }

    closedir(unixdir);

    return list;
}
