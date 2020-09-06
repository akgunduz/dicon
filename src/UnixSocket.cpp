//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "UnixSocket.h"
#include "NetUtil.h"

UnixSocket::UnixSocket(const TypeHostUnit& host, const TypeDevice& device, const InterfaceSchedulerCB *schedulerCB)
        : CommInterface(host, device, schedulerCB) {

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

        sockaddr_un serverAddress = NetUtil::getUnixAddress(newAddress);

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

#ifndef WIN32
        if (fcntl(unixSocket, F_SETFD, O_NONBLOCK) < 0) {
#else
        u_long nonblocking_enabled = true;
        if (ioctlsocket( unixSocket, FIONBIO, &nonblocking_enabled ) != 0) {
#endif
            LOGS_E(getHost(), "Could not set socket Non-Blocking!!!");
            close(unixSocket);
            return false;
        }

        setAddress(newAddress);

        LOGS_I(getHost(), "Using address : %s", NetUtil::getUnixString(newAddress.get()).c_str());

        return true;
    }

    LOGS_E(getHost(), "Could not set create unix socket!!!");

    close(unixSocket);

    return false;
}

bool UnixSocket::runReceiver() {

    bool thread_started = true;
    std::thread threadAccept;

    int maxfd = std::max(unixSocket, notifierPipe[0]) + 1;

    fd_set readfs, orjreadfs;

    FD_ZERO(&orjreadfs);
    FD_SET(unixSocket, &orjreadfs);
    FD_SET(notifierPipe[0], &orjreadfs);

    while (thread_started) {

        readfs = orjreadfs;

        int nready = select(maxfd, &readfs, nullptr, nullptr, nullptr);
        if (nready == -1) {
            LOGS_E(getHost(), "Problem with select call with err : %d!!!", errno);
            return false;
        }

        if (FD_ISSET(unixSocket, &readfs)) {

            int acceptSocket = accept(unixSocket, nullptr, nullptr);
            if (acceptSocket < 0) {
                LOGS_E(getHost(), "Node Socket open with err : %d!!!", errno);
                return false;
            }

            threadAccept = std::thread([](CommInterface *commInterface, int acceptSocket) {

                auto source = std::make_shared<ComponentUnit>(acceptSocket);

                auto msg = std::make_unique<Message>(commInterface->getHost());

                if (msg->readData(source)) {

                    auto owner = msg->getHeader().getOwner();

                    commInterface->push(MSGDIR_RECEIVE, owner, std::move(msg));
                }
            }, this, acceptSocket);
            threadAccept.detach();
        }

        if (FD_ISSET(notifierPipe[0], &readfs)) {

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

bool UnixSocket::runSender(const TypeComponentUnit& target, TypeMessage msg) {

    int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
        return false;
    }

    sockaddr_un clientAddress = NetUtil::getUnixAddress(target->getAddress());

    socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t) strlen(clientAddress.sun_path);

    if (connect(clientSocket, (struct sockaddr *) &clientAddress, len) == -1) {
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

bool UnixSocket::runMulticastSender(const TypeComponentUnit& target, TypeMessage message) {

    return false;
}

TypeReadCB UnixSocket::getReadCB(const TypeComponentUnit& source) {

    return [] (const TypeComponentUnit& source, uint8_t *buf, size_t size) -> size_t {

        return read(source->getSocket(), buf, size);
    };
}

TypeWriteCB UnixSocket::getWriteCB(const TypeComponentUnit& source) {

    return [] (const TypeComponentUnit& target, const uint8_t *buf, size_t size) -> size_t {

        return write(target->getSocket(), buf, size);
    };
}

UnixSocket::~UnixSocket() {

    LOGP_T("Deallocating UnixSocket");

    end();

    close(unixSocket);
}

COMM_INTERFACE UnixSocket::getType() {

    return COMMINTERFACE_UNIXSOCKET;

}

bool UnixSocket::isSupportMulticast() {

    return false;
}

TypeAddressList UnixSocket::getAddressList() {

    TypeAddressList list;

    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::temp_directory_path())) {

        if (!entry.is_regular_file() || entry.path().filename().string().find(UNIXSOCKET_FILE_PREFIX) != 0) {
            continue;
        }

        uint32_t start = (uint32_t) entry.path().filename().string().find('_') + 1;
        uint32_t end = (uint32_t) entry.path().filename().string().find('.');
        std::string sAddress = entry.path().filename().string().substr(start, end - start);
        Address address(atol(sAddress.c_str()), 0);

        list.push_back(address);
    }

    return list;
}
