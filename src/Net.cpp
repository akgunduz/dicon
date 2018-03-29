//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Net.h"
#include "NetAddress.h"

std::vector<Device> Net::deviceList;

Net::Net(Unit host, Device *device, const InterfaceCallback *cb)
		: Interface(host, device, cb) {

    if (!initTCP()) {
        LOG_E("initTCP failed!!!");
        throw std::runtime_error("NetReceiver : initTCP failed!!!");
    }

    if (!initMulticast()) {
        LOG_E("initMulticast failed!!!");
        throw std::runtime_error("NetReceiver : initMulticast failed!!!");
    }

    if (!initThread()) {
        LOG_E("initMulticast failed!!!");
        throw std::runtime_error("NetReceiver : initThread failed!!!");
    }

}

bool Net::initTCP() {

    netSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (netSocket < 0) {
        LOG_E("Socket open with err : %d!!!", errno);
        return false;
    }

    int on = 1;
    if (setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(netSocket);
        return false;
    }

    int tryCount = 10;

    for (int j = tryCount; j > 0; j--) {

        setAddress(lastFreePort);

        struct sockaddr_in serverAddress = NetAddress::getInetAddress(getAddress());

        if (bind(netSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {

            lastFreePort++;
            continue;
        }

        LOG_I("Using address : %s", Address::getString(address).c_str());

        if (listen(netSocket, MAX_SIMUL_CLIENTS) < 0) {
            LOG_E("Socket listen with err : %d!!!", errno);
            close(netSocket);
            return false;
        }

        if(fcntl(netSocket, F_SETFD, O_NONBLOCK) < 0) {
            LOG_E("Could not set socket Non-Blocking!!!");
            close(netSocket);
            return false;
        }

        device->setPort(lastFreePort);

        return true;
    }


    LOG_E("Could not set create tcp net socket!!!");

    close(netSocket);

    return false;
}

bool Net::initMulticast() {

    multicastAddress = NetAddress::parseAddress(MULTICAST_ADDRESS, DEFAULT_MULTICAST_PORT, 0);

    multicastSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (multicastSocket < 0) {
        LOG_E("Socket receiver open with err : %d!!!", errno);
        return false;
    }

    int on = 1;
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(int)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    struct sockaddr_in serverAddress = NetAddress::getInetAddress(DEFAULT_MULTICAST_PORT);
    if (bind(multicastSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {
        LOG_E("Socket bind with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    LOG_I("Using multicast address : %s", Address::getString(multicastAddress).c_str());

    ip_mreq imreq = NetAddress::getMulticastAddress(address);

    if (setsockopt(multicastSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *) &imreq, sizeof(ip_mreq)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }
/*
        if (setsockopt(multicastSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (const void *) &on, sizeof(int)) < 0) {
            LOG_E("Socket option with err : %d!!!", errno);
            close(multicastSocket);
            return false;
        }
        */

    return true;
}

void Net::runReceiver(Unit host) {

    bool thread_started = true;

    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    fd_set readfs, orjreadfs;
    FD_ZERO(&orjreadfs);

    int maxfd = std::max(netSocket, notifierPipe[0]);
    maxfd = std::max(maxfd, multicastSocket);
    maxfd++;

    FD_SET(multicastSocket, &orjreadfs);
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
				LOG_E("Node Socket open with err : %d!!!", errno);
				return;
			}

			Argument *argument = new Argument(this, cli_addr.sin_addr.s_addr);
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

        if (FD_ISSET(multicastSocket, &readfs)) {

            Message *msg = new Message(host);
            msg->setMulticastAddress(0xFFFF);
            if (msg->readFromStream(multicastSocket)) {
                push(MESSAGE_RECEIVE, msg->getHeader()->getOwnerAddress(), msg);
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

	Argument *argument = (Argument *) arg;

	Message *msg = new Message(argument->host);
	if (msg->readFromStream(argument->acceptSocket)) {
		argument->_interface->push(MESSAGE_RECEIVE, msg->getHeader()->getOwnerAddress(), msg);
	}

	delete argument;
	return nullptr;
}

void Net::runSender(long target, Message *msg) {

	int clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
        LOG_T("Socket sender open with err : %d!!!", errno);
		return;
	}

	LOG_T("Socket sender %d is opened !!!", clientSocket);

    sockaddr_in clientAddress = NetAddress::getInetAddress(target);

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(sockaddr_in)) == -1) {
        LOG_T("Socket can not connect!!!");
		close(clientSocket);
		return;
	}

	LOG_T("Socket sender %d is connected !!!", clientSocket);

	msg->getHeader()->setOwnerAddress(address);
	msg->writeToStream(clientSocket);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

void Net::runMulticastSender(Message *msg) {

    int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) {
        LOG_T("Socket sender open with err : %d!!!", errno);
        return;
    }

    struct in_addr interface_addr = NetAddress::getInetAddress(address).sin_addr;
    setsockopt(clientSocket, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr, sizeof(interface_addr));

    msg->getHeader()->setOwnerAddress(address);
    msg->setMulticastAddress(multicastAddress);
    msg->writeToStream(clientSocket);

    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
}

void Net::setAddress(unsigned short port) {

    address = NetAddress::parseAddress(device->getAddress(),
			port, (int) device->getHelper());
}

Net::~Net() {
	end();
    close(multicastSocket);
	close(netSocket);
}


INTERFACES Net::getType() {

	return INTERFACE_NET;

}

std::vector<long> Net::getAddressList() {

    return NetAddress::getAddressList(address);
}

bool Net::createDevices() {

    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return false;
    }

    for (loop = ifAddrStruct; loop != NULL; loop = loop->ifa_next) {

        if (loop->ifa_addr->sa_family == AF_INET) { // check it is IP4

            if (!((loop->ifa_flags & IFF_UP) && (loop->ifa_flags & IFF_RUNNING))) {
                continue;
            }

            deviceList.push_back(Device(loop->ifa_name,
                                        ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                        NetAddress::address2prefix(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)),
                                        (loop->ifa_flags & IFF_LOOPBACK) > 0));
        }
    };

    freeifaddrs(ifAddrStruct);

    return true;
}

std::vector<Device>* Net::getDevices() {

    if (deviceList.size() == 0) {
        createDevices();
    }
    return &deviceList;
}

bool Net::isSupportMulticast() {

    return Util::isMulticast();
}
