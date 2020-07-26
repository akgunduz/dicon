//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Net.h"
#include "Util.h"

Net::Net(Device *device, const InterfaceSchedulerCB *scb, const InterfaceHostCB *hcb)
		: Interface(device, scb, hcb) {

    if (!initTCP()) {
        LOGS_E(getHost(), "initTCP failed!!!");
        throw std::runtime_error("Net : initTCP failed!!!");
    }

    if (!initMulticast()) {
        LOGS_E(getHost(), "initMulticast failed!!!");
        throw std::runtime_error("Net : initMulticast failed!!!");
    }

    if (!initThread()) {
        LOGS_E(getHost(), "initThread failed!!!");
        throw std::runtime_error("Net : initThread failed!!!");
    }

}

bool Net::initTCP() {

    netSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (netSocket < 0) {
        LOGS_E(getHost(), "Socket open with err : %d!!!", errno);
        return false;
    }

    int on = 1;
    if (setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(netSocket);
        return false;
    }

    int tryCount = 10;
    int lastFreePort = DEFAULT_PORT;

    for (int j = tryCount; j > 0; j--) {

        long new_address = Address::create(getDevice()->getType(), getDevice()->getBase(), lastFreePort);

        struct sockaddr_in serverAddress = getInetAddressByAddress(new_address);

        if (bind(netSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {

            lastFreePort++;
            continue;
        }

        if (listen(netSocket, MAX_SIMUL_CLIENTS) < 0) {
            LOGS_E(getHost(), "Socket listen with err : %d!!!", errno);
            close(netSocket);
            return false;
        }

        if(fcntl(netSocket, F_SETFD, O_NONBLOCK) < 0) {
            LOGS_E(getHost(), "Could not set socket Non-Blocking!!!");
            close(netSocket);
            return false;
        }

        setAddress(new_address);

        LOGS_T(getHost(), "Using address : %s", InterfaceTypes::getAddressString(new_address).c_str());

        return true;
    }


    LOGS_E(getHost(), "Could not set create tcp net socket!!!");

    close(netSocket);

    return false;
}

bool Net::initMulticast() {

    long multicastAddress = Address::create(getDevice()->getType(), MULTICAST_ADDRESS, DEFAULT_MULTICAST_PORT);

    multicastSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (multicastSocket < 0) {
        LOGS_E(getHost(), "Socket receiver open with err : %d!!!", errno);
        return false;
    }

    int on = 1;
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(int)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    struct sockaddr_in serverAddress = getInetAddressByPort(DEFAULT_MULTICAST_PORT);
    if (bind(multicastSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {
        LOGS_E(getHost(), "Socket bind with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    ip_mreq imreq = getInetMulticastAddress(getAddress());

    if (setsockopt(multicastSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *) &imreq, sizeof(ip_mreq)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    setMulticastAddress(multicastAddress);

    LOGS_T(getHost(), "Using multicast address : %s", InterfaceTypes::getAddressString(multicastAddress).c_str());

    return true;
}

void Net::runReceiver() {

    bool thread_started = true;
    std::thread threadAccept;

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
			LOGS_E(getHost(), "Problem with select call with err : %d!!!", errno);
			return;
		}

		if (FD_ISSET(netSocket, &readfs)) {

			int acceptSocket = accept(netSocket, (struct sockaddr *) &cli_addr, &clilen);
			if (acceptSocket < 0) {
				LOGS_E(getHost(), "Node Socket open with err : %d!!!", errno);
				return;
			}

            threadAccept = std::thread(runAccepter, this, acceptSocket);
            threadAccept.detach();

		}

        if (FD_ISSET(multicastSocket, &readfs)) {

            auto *msg = new Message(getHost());
            sockaddr_in address{};
            address.sin_addr.s_addr = 1;
            msg->setDatagramAddress(address);
            if (msg->readFromStream(multicastSocket)) {
                push(MESSAGE_RECEIVE, msg->getHeader().getOwner().getAddress(), msg);
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

void Net::runAccepter(Interface *interface, int acceptSocket) {

	auto *msg = new Message(interface->getHost());

	if (msg->readFromStream(acceptSocket)) {

		interface->push(MESSAGE_RECEIVE, msg->getHeader().getOwner().getAddress(), msg);
	}
}

void Net::runSender(long target, Message *msg) {

	int clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
		return;
	}

	LOGS_T(getHost(), "Sender is opened for target : %s and message : %s !!!",
          InterfaceTypes::getAddressString(target).c_str(), MessageTypes::getMsgName(msg->getHeader().getType()));

    sockaddr_in clientAddress = getInetAddressByAddress(target);

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(sockaddr_in)) == -1) {
        LOGS_E(getHost(), "Socket can not connect!!!");
		close(clientSocket);
		return;
	}

    LOGS_T(getHost(), "Sender is connected for target : %s and message : %s !!!",
          InterfaceTypes::getAddressString(target).c_str(), MessageTypes::getMsgName(msg->getHeader().getType()));

	msg->writeToStream(clientSocket);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

void Net::runMulticastSender(Message *msg) {

    int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
        return;
    }

    struct in_addr interface_addr = getInetAddressByAddress(getAddress()).sin_addr;
    setsockopt(clientSocket, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr, sizeof(interface_addr));

    msg->setDatagramAddress(getInetAddressByAddress(getMulticastAddress()));
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

bool Net::isSupportMulticast() {

    return Util::isMulticast();
}

std::string Net::getAddressString(long address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%d", getIPString(address).c_str(), Address::getPort(address));
    return std::string(sAddress);
}

std::string Net::getIPString(long address) {

    struct in_addr addr{};
    addr.s_addr = htonl(Address::getBase(address));
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        return "";
    }

    return std::string(cIP);
}

long Net::parseIPAddress(const std::string& address) {

    struct in_addr addr{};

    int res = inet_pton(AF_INET, address.c_str(), &addr);
    if (res <= 0) {
        return 0;
    }

    return ntohl(addr.s_addr);
}

long Net::parseAddress(std::string address) {

    long pos = address.find(':');

    std::string sIP = address.substr(0, pos);

    std::string sPort = address.substr(pos + 1, std::string::npos);

    long ip = parseIPAddress(sIP);
    int port = atoi(sPort.c_str());

    return Address::create(INTERFACE_NET, ip, port);
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

            long destAddress = Address::create(device->getType(), device->getBase(), DEFAULT_PORT + i);

            if (destAddress != device->getBase()) {

                list.push_back(destAddress);

            }

        }

    } else {

        long range = (1 << (32 - device->getMask())) - 2;

        int mask = ((int)0x80000000) >> (device->getMask() - 1);

        long net = mask & device->getBase();

        long startIP = net + 1;

        for (int i = 0; i < range; i++) {

            if (startIP != device->getBase()) {

                long destAddress = Address::create(device->getType(), startIP, DEFAULT_PORT);

                list.push_back(destAddress);

            }

            startIP++;
        }

    }

    return list;
}
