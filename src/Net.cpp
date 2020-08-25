//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Net.h"
#include "NetUtil.h"

Net::Net(const TypeHostUnit& host, const TypeDevice& device, const InterfaceSchedulerCB *schedulerCB)
        : CommInterface(host, device, schedulerCB) {

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
    if (setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(netSocket);
        return false;
    }

    int tryCount = 10;
    int lastFreePort = DEFAULT_PORT;

    for (int j = tryCount; j > 0; j--) {

        Address new_address(getDevice()->getBase(), lastFreePort);

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByAddress(new_address);

        if (bind(netSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {

            lastFreePort++;
            continue;
        }

        if (listen(netSocket, MAX_SIMUL_CLIENTS) < 0) {
            LOGS_E(getHost(), "Socket listen with err : %d!!!", errno);
            close(netSocket);
            return false;
        }

#ifndef WIN32
        if (fcntl(netSocket, F_SETFD, O_NONBLOCK) < 0) {
#else
        u_long nonblocking_enabled = true;
        if (ioctlsocket( netSocket, FIONBIO, &nonblocking_enabled ) != 0) {
#endif
            LOGS_E(getHost(), "Could not set socket Non-Blocking!!!");
            close(netSocket);
            return false;
        }

        setAddress(new_address);

        LOGS_T(getHost(), "Using address : %s", NetUtil::getIPPortString(new_address.get()).c_str());

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
    if (setsockopt(multicastSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    struct sockaddr_in serverAddress = NetUtil::getInetAddressByPort(DEFAULT_MULTICAST_PORT);
    if (bind(multicastSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {
        LOGS_E(getHost(), "Socket bind with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    ip_mreq imreq = NetUtil::getInetMulticastAddress(getAddress(), MULTICAST_ADDRESS);

    if (setsockopt(multicastSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) &imreq, sizeof(ip_mreq)) < 0) {
        LOGS_E(getHost(), "Socket option with err : %d!!!", errno);
        close(multicastSocket);
        return false;
    }

    multicastAddress.setMulticast(true);
    setMulticastAddress(multicastAddress);

    LOGS_T(getHost(), "Using multicast address : %s", NetUtil::getIPPortString(multicastAddress.get()).c_str());

    return true;
}

TypeReadCB Net::getReadCB(const TypeComponentUnit& source) {

    if (!source->getAddress().isMulticast()) {

        return [] (const TypeComponentUnit& source, uint8_t *buf, size_t size) -> size_t {

            return read(source->getSocket(), buf, size);
        };
    }

    return [] (const TypeComponentUnit& source, uint8_t *buf, size_t size) -> size_t {

        return recvfrom(source->getSocket(), (char*)buf, size, 0, nullptr, nullptr);
    };
}

TypeWriteCB Net::getWriteCB(const TypeComponentUnit& target) {

    if (!target->getAddress().isMulticast()) {

        return [] (const TypeComponentUnit& target, const uint8_t *buf, size_t size) -> size_t {

            return write(target->getSocket(), buf, size);
        };
    }

    return [] (const TypeComponentUnit& target, const uint8_t *buf, size_t size) -> size_t {

        struct sockaddr_in datagramAddress = NetUtil::getInetAddressByAddress(target->getAddress());

        return sendto(target->getSocket(), (const char*)buf, size, 0,
                      (struct sockaddr *) &datagramAddress, sizeof(struct sockaddr));
    };
}

bool Net::runReceiver() {

    bool thread_started = true;
    std::thread threadAccept;

    struct sockaddr_in cli_addr{};

    socklen_t cliLen = sizeof(cli_addr);

    fd_set readFS, orjReadFS;
    FD_ZERO(&orjReadFS);

    int maxfd = std::max(netSocket, notifierPipe[0]);
    maxfd = std::max(maxfd, multicastSocket);
    maxfd++;

    FD_SET(multicastSocket, &orjReadFS);
    FD_SET(netSocket, &orjReadFS);
    FD_SET(notifierPipe[0], &orjReadFS);

    while (thread_started) {

        readFS = orjReadFS;

        int nReady = select(maxfd, &readFS, nullptr, nullptr, nullptr);
        if (nReady == -1) {
            LOGS_E(getHost(), "Problem with select call with err : %d!!!", errno);
            return false;
        }

        if (FD_ISSET(netSocket, &readFS)) {

            int acceptSocket = accept(netSocket, (struct sockaddr *) &cli_addr, &cliLen);
            if (acceptSocket < 0) {
                LOGS_E(getHost(), "Node Socket open with err : %d!!!", errno);
                return false;
            }

            threadAccept = std::thread([](CommInterface *commInterface, int acceptSocket) {

                auto source = std::make_shared<ComponentUnit>(acceptSocket);

                auto msg = std::make_unique<Message>(commInterface->getHost());

                if (msg->readFromStream(source)) {

                    auto owner = msg->getHeader().getOwner();

                    commInterface->push(MSGDIR_RECEIVE, owner, std::move(msg));
                }

                close(acceptSocket);

            }, this, acceptSocket);

            threadAccept.detach();
        }

        if (FD_ISSET(multicastSocket, &readFS)) {

            auto source = std::make_shared<ComponentUnit>(multicastSocket);
            source->getAddress().setMulticast(true);

            auto msg = std::make_unique<Message>(getHost());

            if (msg->readFromStream(source)) {

                auto owner = msg->getHeader().getOwner();

                push(MSGDIR_RECEIVE, owner, std::move(msg));
            }
        }

        if (FD_ISSET(notifierPipe[0], &readFS)) {

            char data;
            read(notifierPipe[0], &data, 1);
            switch (data) {
                case SHUTDOWN_NOTIFIER:
                    thread_started = false;
                    break;
                default:
                    break;
            }
        }
    }

    return true;
}

bool Net::runSender(const TypeComponentUnit& target, TypeMessage msg) {

    int clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
        return false;
    }

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    if (connect(clientSocket, (struct sockaddr *) &clientAddress, sizeof(sockaddr_in)) == -1) {
        LOGS_E(getHost(), "Socket can not connect!!!");
        close(clientSocket);
        return false;
    }

    target->setSocket(clientSocket);

    msg->writeToStream(target);

    shutdown(clientSocket, SHUT_RDWR);

    close(clientSocket);

    return true;
}

bool Net::runMulticastSender(const TypeComponentUnit& target, TypeMessage msg) {

    int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
        return false;
    }

    struct in_addr interface_addr = NetUtil::getInetAddressByAddress(getAddress()).sin_addr;
    setsockopt(clientSocket, IPPROTO_IP, IP_MULTICAST_IF, (const char*)&interface_addr, sizeof(interface_addr));

    target->setSocket(clientSocket);

    msg->writeToStream(target);

    shutdown(clientSocket, SHUT_RDWR);

    close(clientSocket);

    return true;
}

Net::~Net() {

    LOGP_T("Deallocating Net");

    end();

    close(multicastSocket);

    close(netSocket);
}

COMM_INTERFACE Net::getType() {

    return COMMINTERFACE_TCPIP;
}

bool Net::isSupportMulticast() {

    return true;
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

        uint32_t mask = ((int) 0x80000000) >> (getDevice()->getMask() - 1);

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
