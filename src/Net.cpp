//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Net.h"

std::vector<Device*> Net::deviceList;

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
    int lastFreePort = DEFAULT_PORT;

    for (int j = tryCount; j > 0; j--) {

        long address = Address::createAddress(getDevice()->getType(),
                                              getDevice()->getBase(), lastFreePort, getDevice()->getHelper());

        struct sockaddr_in serverAddress = getInetAddressByAddress(address);

        if (bind(netSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {

            lastFreePort++;
            continue;
        }

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

        setAddress(address);

        LOG_I("Using address : %s", InterfaceTypes::getAddressString(address).c_str());

        return true;
    }


    LOG_E("Could not set create tcp net socket!!!");

    close(netSocket);

    return false;
}

bool Net::initMulticast() {

    long multicastAddress = Address::createAddress(getDevice()->getType(), MULTICAST_ADDRESS, DEFAULT_MULTICAST_PORT, 0);

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

    struct sockaddr_in serverAddress = getInetAddressByPort(DEFAULT_MULTICAST_PORT);
    if (bind(multicastSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {
        LOG_E("Socket bind with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    ip_mreq imreq = getInetMulticastAddress(getAddress());

    if (setsockopt(multicastSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *) &imreq, sizeof(ip_mreq)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    setMulticastAddress(multicastAddress);

    LOG_I("Using multicast address : %s", getAddressString(multicastAddress).c_str());

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
            sockaddr_in address;
            address.sin_addr.s_addr = 1;
            msg->setMulticastAddress(address);
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

    sockaddr_in clientAddress = getInetAddressByAddress(target);

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(sockaddr_in)) == -1) {
        LOG_T("Socket can not connect!!!");
		close(clientSocket);
		return;
	}

	LOG_T("Socket sender %d is connected !!!", clientSocket);

	msg->getHeader()->setOwnerAddress(getAddress());
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

    struct in_addr interface_addr = getInetAddressByAddress(getAddress()).sin_addr;
    setsockopt(clientSocket, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr, sizeof(interface_addr));

    msg->getHeader()->setOwnerAddress(getAddress());
    msg->setMulticastAddress(getInetAddressByAddress(getMulticastAddress()));
    msg->writeToStream(clientSocket);

    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
}

Net::~Net() {
	end();
    close(multicastSocket);
	close(netSocket);
}


INTERFACE Net::getType() {

	return INTERFACE_NET;

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

            Device *device = new Device(Device(loop->ifa_name, INTERFACE_NET,
                                               ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                               address2prefix(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)),
                                               (loop->ifa_flags & IFF_LOOPBACK) > 0));

            deviceList.push_back(device);
        }
    };

    freeifaddrs(ifAddrStruct);

    return true;
}

std::vector<Device*>* Net::getDevices() {

    if (deviceList.size() == 0) {
        createDevices();
    }
    return &deviceList;
}

bool Net::isSupportMulticast() {

    return Util::isMulticast();
}

std::string Net::getAddressString(long address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%d", getIPString(address).c_str(), Address::getPort(address));
    return std::string(sAddress);
}

int Net::address2prefix(long address) {

    int i = 0;
    uint32_t ip = (uint32_t) Address::getBase(address);
    while(ip > 0) {
        ip = ip >> 1;
        i++;
    }

    return i;
}

std::string Net::getIPString(long address) {

    struct in_addr addr;
    addr.s_addr = htonl(Address::getBase(address));
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        return "";
    }

    return std::string(cIP);
}

sockaddr_in Net::getInetAddressByAddress(long address) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons((uint16_t)Address::getPort(address));
    inet_addr.sin_addr.s_addr = htonl((uint32_t)Address::getBase(address));
    return inet_addr;
}

sockaddr_in Net::getInetAddressByPort(int port) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(port);
    inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return inet_addr;
}

ip_mreq Net::getInetMulticastAddress(long address) {

    ip_mreq imreq;
    memset((char *) &imreq, 0, sizeof(imreq));

    imreq.imr_multiaddr.s_addr = htonl(MULTICAST_ADDRESS);
    imreq.imr_interface.s_addr = htonl((uint32_t)Address::getBase(address));
    return imreq;

}

std::vector<long> Net::getAddressList(Device* device) {

    std::vector<long> list;

    if (device->isLoopback()) {

        for (int i = 0; i < LOOPBACK_RANGE; i++) {

            long destAddress = Address::createAddress(device->getType(),
                                                      device->getBase(), DEFAULT_PORT + i, device->getHelper());

            if (destAddress != device->getBase()) {

                list.push_back(destAddress);

            }

        }

    } else {

        long range = (1 << (32 - device->getHelper())) - 2;

        int mask = ((int)0x80000000) >> (device->getHelper() - 1);

        long net = mask & device->getBase();

        long startIP = net + 1;

        for (int i = 0; i < range; i++) {

            if (startIP != device->getBase()) {

                long destAddress = Address::createAddress(device->getType(),
                                                          startIP, DEFAULT_PORT, device->getHelper());

                list.push_back(destAddress);

            }

            startIP++;
        }

    }

    return list;
}