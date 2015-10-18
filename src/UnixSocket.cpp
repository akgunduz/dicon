//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "UnixSocket.h"
#include "UnixSocketAddress.h"

uint16_t UnixSocket::gOffset = 1001;
std::vector<ConnectInterface> UnixSocket::interfaceList;

UnixSocket::UnixSocket(Unit host, int interfaceIndex, const InterfaceCallback *cb, const char *rootPath)
		: Interface(host, INTERFACE_UNIXSOCKET, cb, rootPath) {

	if (!init(interfaceIndex)) {
		LOG_E("Instance create failed!!!");
		throw std::runtime_error("NetReceiver : Instance create failed!!!");
	}
	LOG_I("Instance is created, Socket : %d!!!", unixSocket);

}

bool UnixSocket::init(int interfaceIndex) {

	unixSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (unixSocket < 0) {
		LOG_E("Socket receiver open with err : %d!!!", errno);
		return false;
	}

	setAddress(interfaceIndex);

    sockaddr_un serverAddress = UnixSocketAddress::getUnixAddress(address);

	socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(serverAddress.sun_path);

	int on = 1;
	if (setsockopt(unixSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
		LOG_E("Socket option with err : %d!!!", errno);
		close(unixSocket);
		return false;
	}
	LOG_T("Socket set option is OK!!!");

	if (bind(unixSocket, (struct sockaddr *) &serverAddress, len) < 0) {
		LOG_E("Socket bind with err : %d!!!", errno);
		close(unixSocket);
		return false;
	}
	LOG_T("Socket binding is OK!!!");

	if (listen(unixSocket, MAX_SIMUL_CLIENTS) < 0) {
		LOG_E("Socket listen with err : %d!!!", errno);
		close(unixSocket);
		return false;
	}
	LOG_T("Socket listen is OK!!!");

	if(fcntl(unixSocket, F_SETFD, O_NONBLOCK) < 0) {
		LOG_E("Could not set socket Non-Blocking!!!");
		close(unixSocket);
		return false;
	}
	LOG_T("Set socket Non-Blocking is OK!!!");

	if (!initThread()) {
		LOG_E("Problem with Server thread");
		close(unixSocket);
		return false;
	}

	return true;
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

	Message *msg = new Message(argument->host, argument->_interface->getRootPath());
	if (msg->readFromStream(argument->acceptSocket)) {
		argument->_interface->push(MESSAGE_RECEIVE, msg->getOwnerAddress(), msg);
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

    sockaddr_un clientAddress = UnixSocketAddress::getUnixAddress(target);

	socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(clientAddress.sun_path);

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, len) == -1) {
		LOG_E("Socket can not connect!!!");
		close(clientSocket);
		return;
	}

	LOG_T("Socket sender %d is connected !!!", clientSocket);

	msg->setOwnerAddress(address);
	msg->writeToStream(clientSocket);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

UnixSocket::~UnixSocket() {
	end();
	close(unixSocket);
}


void UnixSocket::setAddress(int index) {

	address = (((unsigned)getpid() << 10) & 0xFFFFFF) |  gOffset++;
}

INTERFACES UnixSocket::getType() {

	return INTERFACE_UNIXSOCKET;

}

std::vector<long> UnixSocket::getAddressList() {

	std::vector<long> list;

	DIR *unixdir = opendir(UNIXSOCKET_PATH);
	if (!unixdir) {
		printf("Can not open unix socket path!!!");
		return list;
	}

	std::string path1 = "";

	path1 = UNIXSOCKET_FILE_PREFIX;
	path1.append(Address::getString(address));
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
		long address = (unsigned)atol(saddress.c_str());

		list.push_back(address);

	}

	closedir(unixdir);

	return list;
}

std::vector<ConnectInterface> UnixSocket::getInterfaces() {

    if (interfaceList.size() > 0) {
        return interfaceList;
    }

    interfaceList.push_back(ConnectInterface("us", INTERFACE_UNIXSOCKET));

    return interfaceList;
}
