//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Net.h"
#include "Util.h"

Net::Net(Device *device, const InterfaceSchedulerCB *scb, const InterfaceHostCB *hcb)
		: Interface(device, scb, hcb) {

    if (!initTCP()) {
        throw std::runtime_error("Net : initTCP failed!!!");
    }

    if (!initMulticast()) {
        throw std::runtime_error("Net : initMulticast failed!!!");
    }

    if (!initThread()) {
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

        Address new_address(getDevice()->getBase(), lastFreePort);

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

        LOGS_T(getHost(), "Using address : %s", getAddressString(new_address).c_str());

        return true;
    }


    LOGS_E(getHost(), "Could not set create tcp net socket!!!");

    close(netSocket);

    return false;
}

bool Net::initMulticast() {

    Address multicastAddress(MULTICAST_ADDRESS, DEFAULT_MULTICAST_PORT);

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

    multicastAddress.setMulticast(true);
    setMulticastAddress(multicastAddress);

    LOGS_T(getHost(), "Using multicast address : %s", getAddressString(multicastAddress).c_str());

    return true;
}

size_t Net::readCB(ComponentUnit& source, uint8_t * buf, size_t size) {

    return read(source.getSocket(), buf, size);
}

size_t Net::readMulticastCB(ComponentUnit& source, uint8_t* buf, size_t size) {

    return recvfrom(source.getSocket(), buf, size, 0, nullptr, nullptr);
}

TypeReadCB Net::getReadCB(ComponentUnit& source) {

    if (!source.getAddress().isMulticast()) {

        return readCB;
    }

    return readMulticastCB;
}

size_t Net::writeCB(ComponentUnit& target, const uint8_t * buf, size_t size) {

    return write(target.getSocket(), buf, size);
}

size_t Net::writeMulticastCB(ComponentUnit& target, const uint8_t* buf, size_t size) {

    struct sockaddr_in datagramAddress = getInetAddressByAddress(target.getAddress());

    return sendto(target.getSocket(), buf, size, 0,
                  (struct sockaddr *) &datagramAddress, sizeof(struct sockaddr));
}

TypeWriteCB Net::getWriteCB(ComponentUnit& target) {

    if (!target.getAddress().isMulticast()) {

        return writeCB;
    }

    return writeMulticastCB;
}

void Net::runReceiver() {

    bool thread_started = true;
    std::thread threadAccept;

    struct sockaddr_in cli_addr{};
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

            threadAccept = std::thread(runAcceptor, this, acceptSocket);
            threadAccept.detach();
		}

        if (FD_ISSET(multicastSocket, &readfs)) {

            ComponentUnit source;
            source.setSocket(multicastSocket);
            source.getAddress().setMulticast(true);

            auto *msg = new Message(getHost());
            if (msg->readFromStream(source)) {
                push(MSGDIR_RECEIVE, msg->getHeader().getOwner(), msg);
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

void Net::runAcceptor(Interface *interface, int acceptSocket) {

    ComponentUnit source;
    source.setSocket(acceptSocket);

	auto *msg = new Message(interface->getHost());

	if (msg->readFromStream(source)) {

		interface->push(MSGDIR_RECEIVE, msg->getHeader().getOwner(), msg);
	}
}

void Net::runSender(ComponentUnit target, Message *msg) {

	int clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
		return;
	}

    sockaddr_in clientAddress = getInetAddressByAddress(target.getAddress());

	if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(sockaddr_in)) == -1) {
        LOGS_E(getHost(), "Socket can not connect!!!");
		close(clientSocket);
		return;
	}

	target.setSocket(clientSocket);

	msg->writeToStream(target);

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

void Net::runMulticastSender(ComponentUnit target, Message *msg) {

    int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
        return;
    }

    struct in_addr interface_addr = getInetAddressByAddress(getAddress()).sin_addr;
    setsockopt(clientSocket, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr, sizeof(interface_addr));

    target.setSocket(clientSocket);

    msg->writeToStream(target);

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

std::string Net::getAddressString(Address& address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%u", getIPString(address).c_str(), address.get().port);
    return std::string(sAddress);
}

std::string Net::getIPString(Address& address) {

    struct in_addr addr{};
    addr.s_addr = htonl(address.get().base);
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

Address Net::parseAddress(std::string address) {

    long pos = address.find(':');

    std::string sIP = address.substr(0, pos);

    std::string sPort = address.substr(pos + 1, std::string::npos);

    long ip = parseIPAddress(sIP);
    int port = atoi(sPort.c_str());

    return Address(ip, port);
}


sockaddr_in Net::getInetAddressByAddress(Address& address) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(address.get().port);
    inet_addr.sin_addr.s_addr = htonl(address.get().base);
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

ip_mreq Net::getInetMulticastAddress(Address& address) {

    ip_mreq imreq;
    memset((char *) &imreq, 0, sizeof(imreq));

    imreq.imr_multiaddr.s_addr = htonl(MULTICAST_ADDRESS);
    imreq.imr_interface.s_addr = htonl(address.get().base);
    return imreq;

}

TypeAddressList Net::getAddressList() {

    TypeAddressList list;

    if (getDevice()->isLoopback()) {

        for (int i = 0; i < LOOPBACK_RANGE; i++) {

            Address destAddress(getDevice()->getBase(), DEFAULT_PORT + i);

            if (destAddress != getAddress()) {

                list.push_back(destAddress);

            }

        }

    } else {

        uint32_t range = (1 << (32 - getDevice()->getMask())) - 2;

        uint32_t mask = ((int)0x80000000) >> (getDevice()->getMask() - 1);

        uint32_t net = mask & getDevice()->getBase();

        uint32_t startIP = net + 1;

        for (uint32_t i = 0; i < range; i++) {

            if (startIP != getAddress().get().base) {

                Address destAddress(startIP, DEFAULT_PORT);

                list.push_back(destAddress);

            }

            startIP++;
        }

    }

    return list;
}
