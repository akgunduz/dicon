//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "UnixSocket.h"

UnixSocket::UnixSocket(Device *device, const InterfaceSchedulerCB *scb, const InterfaceHostCB *hcb)
		: Interface(device, scb, hcb) {

	if (!initUnixSocket()) {
		LOGS_E(getHost(), "initUnixSocket failed!!!");
		throw std::runtime_error("UnixSocket : initUnixSocket failed!!!");
	}

    if (!initThread()) {
		LOGS_E(getHost(), "initThread failed!!!");
		throw std::runtime_error("UnixSocket : initThread failed!!!");
    }
}

bool UnixSocket::initUnixSocket() {

    unixSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unixSocket < 0) {
        LOGS_E(getHost(), "Socket receiver open with err : %d!!!", errno);
        return false;
    }

	int tryCount = 10;
    int lastFreePort = 0;

	for (int j = tryCount; j > 0; j--) {

        Address newAddress(getDevice()->getBase(), lastFreePort, getDevice()->getType());

		sockaddr_un serverAddress = getUnixAddress(newAddress);

		socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t) strlen(serverAddress.sun_path);

		if (bind(unixSocket, (struct sockaddr *) &serverAddress, len) < 0) {

			lastFreePort++;
			continue;
		}

		if (listen(unixSocket, MAX_SIMUL_CLIENTS) < 0) {
            LOGS_E(getHost(), "Socket listen with err : %d!!!", errno);
			close(unixSocket);
			return false;
		}

		if (fcntl(unixSocket, F_SETFD, O_NONBLOCK) < 0) {
            LOGS_E(getHost(), "Could not set socket Non-Blocking!!!");
			close(unixSocket);
			return false;
		}

		setAddress(newAddress);

        LOGS_I(getHost(), "Using address : %s", getAddressString(newAddress).c_str());

		return true;
	}

    LOGS_E(getHost(), "Could not set create unix socket!!!");

    close(unixSocket);

    return false;
}

void UnixSocket::runReceiver() {

	bool thread_started = true;
	std::thread threadAccept;

	int maxfd = std::max(unixSocket, notifierPipe[0]) + 1;

	fd_set readfs, orjreadfs;

	FD_ZERO(&orjreadfs);
	FD_SET(unixSocket, &orjreadfs);
	FD_SET(notifierPipe[0], &orjreadfs);

	while(thread_started) {

		readfs = orjreadfs;

		int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
		if (nready == -1) {
			LOGS_E(getHost(), "Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(unixSocket, &readfs)) {

			int acceptSocket = accept(unixSocket, nullptr, nullptr);
			if (acceptSocket < 0) {
				LOGS_E(getHost(), "Node Socket open with err : %d!!!", errno);
				return;
			}

			threadAccept = std::thread(runAcceptor, this, acceptSocket);
			threadAccept.detach();
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

void UnixSocket::runAcceptor(Interface *interface, int acceptSocket) {

    ComponentUnit source;
    source.setSocket(acceptSocket);

	auto *msg = new Message(interface->getHost());

	if (msg->readFromStream(source)) {
		interface->push(MSGDIR_RECEIVE, msg->getHeader().getOwner(), msg);
	}
}

void UnixSocket::runSender(ComponentUnit target, Message *msg) {

	int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
		return;
	}

    sockaddr_un clientAddress = getUnixAddress(target.getAddress());

	socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(clientAddress.sun_path);

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, len) == -1) {
        LOGS_E(getHost(), "Socket can not connect!!!");
		close(clientSocket);
		return;
	}

    target.setSocket(clientSocket);

	msg->writeToStream(target);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

void UnixSocket::runMulticastSender(ComponentUnit target, Message *message) {

}

size_t UnixSocket::readCB(ComponentUnit& source, uint8_t * buf, size_t size) {

    return read(source.getSocket(), buf, size);
}

TypeReadCB UnixSocket::getReadCB(ComponentUnit& source) {

        return readCB;
}

size_t UnixSocket::writeCB(ComponentUnit& target, const uint8_t * buf, size_t size) {

    return write(target.getSocket(), buf, size);
}

TypeWriteCB UnixSocket::getWriteCB(ComponentUnit& source) {

        return writeCB;
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

std::string UnixSocket::getAddressString(Address& address) {

    char sAddress[50];
    sprintf(sAddress, "%u", address.get().base);
    return std::string(sAddress);
}

sockaddr_un UnixSocket::getUnixAddress(Address& address) {

    sockaddr_un unix_addr;
    bzero((char *) &unix_addr, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s%s%u%s", UNIXSOCKET_PATH, UNIXSOCKET_FILE_PREFIX,
            address.get().base, UNIXSOCKET_FILE_SUFFIX);
    return unix_addr;
}

TypeAddressList UnixSocket::getAddressList() {

    TypeAddressList list;

    DIR *unixdir = opendir(UNIXSOCKET_PATH);
    if (!unixdir) {
        //LOG_E("Can not open unix socket path!!!");
        return list;
    }

    dirent *entry;

    while ((entry = readdir(unixdir)) != nullptr) {

        if (strncmp(entry->d_name, UNIXSOCKET_FILE_PREFIX, strlen(UNIXSOCKET_FILE_PREFIX)) != 0) {
            continue;
        }

        std::string path = entry->d_name;

        uint32_t start = (uint32_t)path.find('_') + 1;
        std::string saddress = path.substr(start, path.find('.') - start);
        Address address(atol(saddress.c_str()), 0);

        list.push_back(address);

    }

    closedir(unixdir);

    return list;
}
