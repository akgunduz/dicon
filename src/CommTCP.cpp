//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommTCP.h"
#include "NetUtil.h"
#include "Util.h"

CommTCP::CommTCP(const TypeHostUnit &host, const TypeDevice &device, const InterfaceSchedulerCB *schedulerCB)
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

bool CommTCP::initTCP() {

    uv_tcp_init(&loop, &tcpServer);

    tcpServer.data = this;

    int tryCount = 10;

    //TODO will be enabled later
    //int lastFreePort = DEFAULT_PORT;

    for (int j = tryCount; j > 0; j--) {

        Address new_address(getDevice()->getBase(), lastFreePort);

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByAddress(new_address);

        int result = uv_tcp_bind(&tcpServer, (const struct sockaddr *) &serverAddress, 0);

        if (result < 0 || tcpServer.delayed_error != 0) {

            lastFreePort++;
            continue;
        }

        //TODO will be removed later
        lastFreePort++;

        result = uv_listen((uv_stream_t *) &tcpServer, MAX_SIMUL_CLIENTS,
                           [](uv_stream_t *serverPtr, int status) {

                               auto commInterface = ((CommTCP *) serverPtr->data);

                               if (status < 0) {
                                   LOGS_E(commInterface->getHost(), "Socket listen with err : %d!!!", status);
                                   return;
                               }

                               ((CommTCP *) serverPtr->data)->onConnection();

                           });

        if (result < 0) {

            LOGS_E(getHost(), "Socket listen with err : %d!!!", result);

            return false;
        }

        setAddress(new_address);

        LOGS_T(getHost(), "Using address : %s", NetUtil::getIPPortString(new_address.get()).c_str());

        return true;
    }

    LOGS_E(getHost(), "Could not set create TCP communication!!!");

    return false;
}

bool CommTCP::initMulticast() {

    uv_udp_init(&loop, &multicastServer);

    multicastServer.data = this;

    Address multicastAddress(MULTICAST_ADDRESS, DEFAULT_MULTICAST_PORT);

    struct sockaddr_in serverAddress = NetUtil::getInetAddressByPort(DEFAULT_MULTICAST_PORT);

    uv_udp_bind(&multicastServer, (const struct sockaddr *) &serverAddress, 0);

    uv_udp_set_membership(&multicastServer, NetUtil::getIPString(multicastAddress.get()).c_str(),
                          NetUtil::getIPString(getAddress().get()).c_str(), UV_JOIN_GROUP);

    multicastAddress.setMulticast(true);
    setMulticastAddress(multicastAddress);

    uv_udp_recv_start(&multicastServer, [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {

                          buf->base = (char *) malloc(suggested_size);
                          buf->len = suggested_size;

                      },

                      [](uv_udp_t *client, ssize_t nRead, const uv_buf_t *buf,
                         const struct sockaddr *addr, unsigned flags) {

                          LOGP_E("Data received, count : %d, bufPtr : %s",
                                 nRead, Util::hex2str((uint8_t *) buf->base, nRead).c_str());

                          auto commInterface = (CommTCP *) client->data;

                          commInterface->onRead(commInterface->receiveData[1], nRead, buf);

                      });

    LOGS_T(getHost(), "Using multicast address : %s", NetUtil::getIPPortString(multicastAddress.get()).c_str());

    return true;
}

TypeReadCB CommTCP::getReadCB(const TypeComponentUnit &source) {

    if (!source->getAddress().isMulticast()) {

        return [](const TypeComponentUnit &source, uint8_t *buf, size_t size) -> size_t {

            return read(source->getSocket(), buf, size);
        };
    }

    return [](const TypeComponentUnit &source, uint8_t *buf, size_t size) -> size_t {

        return recvfrom(source->getSocket(), (char *) buf, size, 0, nullptr, nullptr);
    };
}

TypeWriteCB CommTCP::getWriteCB(const TypeComponentUnit &target) {

    if (!target->getAddress().isMulticast()) {

        return [](const TypeComponentUnit &target, const uint8_t *buf, size_t size) -> size_t {

            return write(target->getSocket(), buf, size);
        };
    }

    return [](const TypeComponentUnit &target, const uint8_t *buf, size_t size) -> size_t {

        struct sockaddr_in datagramAddress = NetUtil::getInetAddressByAddress(target->getAddress());

        return sendto(target->getSocket(), (const char *) buf, size, 0,
                      (struct sockaddr *) &datagramAddress, sizeof(struct sockaddr));
    };
}

void CommTCP::onRead(ReceiveData &receiveData, ssize_t nRead, const uv_buf_t *buf) {

    if (receiveData.state == DATASTATE_INIT) {

        receiveData.msg = std::make_unique<Message>(getHost());

        receiveData.unit = std::make_shared<ComponentUnit>();

        receiveData.state = DATASTATE_PROCESS;
    }

    bool isDone = receiveData.msg->onRead(receiveData.unit, nRead, buf);

    if (isDone) {

        receiveData.msg->build(receiveData.unit);

        auto owner = receiveData.msg->getHeader().getOwner();

        push(MSGDIR_RECEIVE, owner, std::move(receiveData.msg));

        receiveData.state = DATASTATE_INIT;
    }
}

bool CommTCP::onConnection() {

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    uv_tcp_init(&loop, client);

    int result = uv_accept((uv_stream_t *) &tcpServer, (uv_stream_t *) client);

    if (result != 0) {

        LOGS_E(getHost(), "Socket accept with err : %d!!!", result);

        return false;
    }

    client->data = this;

    uv_read_start((uv_stream_t *) client,

                  [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {

                      buf->base = (char *) malloc(suggested_size);
                      buf->len = suggested_size;

                  },

                  [](uv_stream_t *client, ssize_t nRead, const uv_buf_t *buf) {

                      auto commInterface = (CommTCP *) client->data;

                      commInterface->onRead(commInterface->receiveData[0], nRead, buf);

                  });

    return true;
}

bool CommTCP::runReceiver() {

    return uv_run(&loop, UV_RUN_DEFAULT);

//    bool thread_started = true;
//    std::thread threadAccept;
//
//    struct sockaddr_in cli_addr{};
//
//    socklen_t cliLen = sizeof(cli_addr);
//
//    fd_set readFS, orjReadFS;
//    FD_ZERO(&orjReadFS);
//
//    int maxfd = std::max(netSocket, notifierPipe[0]);
//    maxfd = std::max(maxfd, multicastSocket);
//    maxfd++;
//
//    FD_SET(multicastSocket, &orjReadFS);
//    FD_SET(netSocket, &orjReadFS);
//    FD_SET(notifierPipe[0], &orjReadFS);
//
//    while (thread_started) {
//
//        readFS = orjReadFS;
//
//        int nReady = select(maxfd, &readFS, nullptr, nullptr, nullptr);
//        if (nReady == -1) {
//            LOGS_E(getHost(), "Problem with select call with err : %d!!!", errno);
//            return false;
//        }
//
//        if (FD_ISSET(netSocket, &readFS)) {
//
//            int acceptSocket = accept(netSocket, (struct sockaddr *) &cli_addr, &cliLen);
//            if (acceptSocket < 0) {
//                LOGS_E(getHost(), "Node Socket open with err : %d!!!", errno);
//                return false;
//            }
//
//            threadAccept = std::thread([](CommInterface *commInterface, int acceptSocket) {
//
//                auto source = std::make_shared<ComponentUnit>(acceptSocket);
//
//                auto msg = std::make_unique<Message>(commInterface->getHost());
//
//                if (msg->readFromStream(source)) {
//
//                    auto owner = msg->getHeader().getOwner();
//
//                    commInterface->push(MSGDIR_RECEIVE, owner, std::move(msg));
//                }
//
//                close(acceptSocket);
//
//            }, this, acceptSocket);
//
//            threadAccept.detach();
//        }
//
//        if (FD_ISSET(multicastSocket, &readFS)) {
//
//            auto source = std::make_shared<ComponentUnit>(multicastSocket);
//            source->getAddress().setMulticast(true);
//
//            auto msg = std::make_unique<Message>(getHost());
//
//            if (msg->readFromStream(source)) {
//
//                auto owner = msg->getHeader().getOwner();
//
//                push(MSGDIR_RECEIVE, owner, std::move(msg));
//            }
//        }
//
//        if (FD_ISSET(notifierPipe[0], &readFS)) {
//
//            char data;
//            read(notifierPipe[0], &data, 1);
//            switch (data) {
//                case SHUTDOWN_NOTIFIER:
//                    thread_started = false;
//                    break;
//                default:
//                    break;
//            }
//        }
//    }

    return true;
}

bool CommTCP::runSender(const TypeComponentUnit &target, TypeMessage msg) {

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

bool CommTCP::runMulticastSender(const TypeComponentUnit &target, TypeMessage msg) {

    int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) {
        LOGS_E(getHost(), "Socket sender open with err : %d!!!", errno);
        return false;
    }

    struct in_addr interface_addr = NetUtil::getInetAddressByAddress(getAddress()).sin_addr;
    setsockopt(clientSocket, IPPROTO_IP, IP_MULTICAST_IF, (const char *) &interface_addr, sizeof(interface_addr));

    target->setSocket(clientSocket);

    msg->writeToStream(target);

    shutdown(clientSocket, SHUT_RDWR);

    close(clientSocket);

    return true;
}

CommTCP::~CommTCP() {

    LOGP_T("Deallocating Net");

    end();

    close(multicastSocket);

    close(netSocket);
}

COMM_INTERFACE CommTCP::getType() {

    return COMMINTERFACE_TCPIP;
}

bool CommTCP::isSupportMulticast() {

    return true;
}

TypeAddressList CommTCP::getAddressList() {

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
