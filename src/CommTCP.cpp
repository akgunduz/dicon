//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommTCP.h"
#include "NetUtil.h"
#include "UvUtil.h"
#include "CommData.h"
#include "Log.h"

CommTCP::CommTCP(TypeHostUnit host, const TypeDevice &device, const CommInterfaceCB *receiverCB)
        : CommInterface(std::move(host), device, receiverCB) {

}

CommTCP::~CommTCP() {

    LOGS_T(getHost(), "Deallocating TCP/UDP Interface");

}

bool CommTCP::initInterface() {

    LOGS_T(getHost(), "Initializing TCP/UDP Interface");

    return initTCP() && initUDP();
}

bool CommTCP::initTCP() {

    tcpServer = (uv_tcp_t*) calloc(1, sizeof(uv_tcp_t));

    int result = uv_tcp_init(&produceLoop, tcpServer);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP socket!!!");

        return false;
    }

    tcpServer->data = new CommData(this);

    int tryCount = TRY_COUNT;

    address->set(COMMINTERFACE_TCPIP, getDevice()->getBase(), lastFreeTCPPort++);

    while (tryCount--) {

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByAddress(address);

        result = uv_tcp_bind(tcpServer, (const struct sockaddr *) &serverAddress, 0);

        if (result < 0 || tcpServer->delayed_error != 0) {

            LOGS_D(getHost(), "Could not bind to socket at address : %s, trying next one!!!",
                   NetUtil::getIPPortString(address->get()).c_str());

            address->setPort(lastFreeTCPPort++);

            continue;
        }

        break;
    }

    if (!tryCount) {

        LOGS_E(getHost(), "Could not bind to socket!!!");

        return false;
    }

    address->setUI();

    result = uv_listen((uv_stream_t *) tcpServer, 1000,

            [](uv_stream_t *server, int status) {

                auto commData = (CommData *) server->data;

                   if (status < 0) {
                    LOGS_E(commData->getInterface()->getHost(), "Socket listen with err : %s", uv_err_name(status));
                    return;
                }

                ((CommTCP *) commData->getInterface())->onServerConnect();

            });

    if (result != 0) {

        LOGS_E(getHost(), "Socket listen with err : %s", uv_err_name(result));

        UvUtil::onClose((uv_handle_t *)tcpServer);

        return false;
    }

    LOGS_T(getHost(), "Using address : %s", NetUtil::getIPPortString(address->get()).c_str());

    return true;
}

bool CommTCP::initUDP() {

    udpServer = (uv_udp_t*) calloc(1, sizeof(uv_udp_t));

    int result = uv_udp_init(&produceLoop, udpServer);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Multicast socket!!!");

        return false;
    }

    int tryCount = TRY_COUNT;

    multicastAddress->set(COMMINTERFACE_TCPIP, MULTICAST_ADDRESS, lastFreeUDPPort, true);

    while (tryCount--) {

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByPort(lastFreeUDPPort);

        result = uv_udp_bind(udpServer, (const struct sockaddr *) &serverAddress, UV_UDP_REUSEADDR);

        if (result < 0) {

            multicastAddress->setPort(++lastFreeUDPPort);

            continue;
        }

        break;
    }

    if (!tryCount) {

        LOGS_E(getHost(), "Could not bind to socket!!!");

        UvUtil::onClose((uv_handle_t*)udpServer);

        return false;
    }

    result = uv_udp_set_membership(udpServer, NetUtil::getIPString(multicastAddress->get()).c_str(),
                          NetUtil::getIPString(address->get()).c_str(), UV_JOIN_GROUP);

    if (result != 0) {

        LOGS_E(getHost(), "Can not join to multicast group!!!");

        UvUtil::onClose((uv_handle_t*)udpServer);

        return false;
    }

    udpServer->data = new CommData(this);

    result = uv_udp_recv_start(udpServer, UvUtil::onAlloc,

            [](uv_udp_t *client, ssize_t nRead, const uv_buf_t *buf,
               const struct sockaddr *addr, unsigned flags) {

                onReceive((uv_handle_t*)client, nRead, buf);
            });

    if (result != 0) {

        LOGS_E(getHost(), "Can not start receiving UDP data!!!");

        UvUtil::onClose((uv_handle_t*)udpServer);

        return false;
    }

    LOGS_T(getHost(), "Using multicast address : %s", NetUtil::getIPPortString(multicastAddress->get()).c_str());

    return true;
}

bool CommTCP::onReceive(uv_handle_t* client, ssize_t nRead, const uv_buf_t *buf) {

    if (nRead == 0) {

        UvUtil::onFree(buf);

        return false;
    }

    if (nRead == UV_EOF || nRead == UV_ECONNRESET) {

        UvUtil::onClose((uv_handle_t*)client);

        UvUtil::onFree(buf);

        return true;
    }

    auto commData = (CommData *) client->data;

    auto commInterface = (CommTCP*)commData->getInterface();

    STATUS status = commInterface->onRead(commData->getComponent(),
                                          commData->getMsg(), (uint8_t *) buf->base, nRead);

    if (status == STATUS_DONE) {

        commData->reInitialize();

    } else if (status == STATUS_SHUTDOWN) {

        commInterface->shutdown();
    }

    UvUtil::onFree(buf);

    return false;
}

bool CommTCP::onTCPSendCB(const TypeComponentUnit &target, const uint8_t *buffer, size_t size) {

    uv_buf_t bufPtr = uv_buf_init((char *) buffer, (unsigned int)size);

    auto *writeReq = (uv_write_t *) calloc(1, sizeof(uv_write_t));

    int result = uv_write(

            writeReq, (uv_stream_t *) target->getHandle(), &bufPtr, 1,

            [](uv_write_t *writeReq, int status) {

                UvUtil::onClose((uv_handle_t*)writeReq->handle);

                free(writeReq);

                if (status) {
                    LOGP_E("TCP Write request problem, error : %d!!!", status);
                }
            });

    if (result != 0) {

        LOGP_E("TCP Write problem, error : %d!!!", result);

        return false;
    }

    return true;
}

bool CommTCP::onUDPSendCB(const TypeComponentUnit &target, const uint8_t *buffer, size_t size) {

    uv_buf_t bufPtr = uv_buf_init((char *) buffer, (unsigned int)size);

    auto *writeReq = (uv_udp_send_t *) calloc(1, sizeof(uv_udp_send_t));

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    int result = uv_udp_send(writeReq, (uv_udp_t *) target->getHandle(), &bufPtr,

        1, (const struct sockaddr *) &clientAddress,

         [](uv_udp_send_t *writeReq, int status) {

             UvUtil::onClose((uv_handle_t*)writeReq->handle);

             free(writeReq);

             if (status) {
                 LOGP_E("Multicast Write request problem, error : %d!!!", status);
             }
         });

    if (result != 0) {

        LOGP_E("Multicast Write problem, error : %d!!!", result);

        return false;
    }
    
    return true;
}

bool CommTCP::onServerConnect() {

    auto *client = (uv_tcp_t *) calloc(1, sizeof(uv_tcp_t));

    int result = uv_tcp_init(&produceLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP client socket!!!");

        return false;
    }

    result = uv_accept((uv_stream_t *) tcpServer, (uv_stream_t *) client);

    if (result != 0) {

        LOGS_E(getHost(), "Socket accept with err : %d!!!", result);

        UvUtil::onClose((uv_handle_t *)&client);

        return false;
    }

    client->data = new CommData(this);

    result = uv_read_start((uv_stream_t *) client, UvUtil::onAlloc,

            [](uv_stream_t *client, ssize_t nRead, const uv_buf_t *buf) {

                onReceive((uv_handle_t*)client, nRead, buf);
            });

    if (result != 0) {

        LOGS_E(getHost(), "Read start with err : %s!!!", uv_err_name(result));

        UvUtil::onClose((uv_handle_t *)&client);

        return false;
    }

    return true;
}

bool CommTCP::onTCPSend(const TypeComponentUnit& target, TypeMessage msg) {

    auto *client = (uv_tcp_t *) calloc(1, sizeof(uv_tcp_t));

    int result = uv_tcp_init(&consumeLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP client socket!!!");

        return false;
    }

    target->setHandle(client);

    client->data = new CommData(this, target, msg);

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    auto *connectReq = (uv_connect_t *) calloc(1, sizeof(uv_connect_t));

    int tryCount = TRY_COUNT;

    while (tryCount--) {

        result = uv_tcp_connect(

                connectReq, client, (const struct sockaddr *) &clientAddress,

                [](uv_connect_t *connectReq, int status) {

                    if (status) {

                        LOGP_E("TCP Connect problem, error : %s!!!", uv_strerror(status));

                        return;
                    }

                    auto commData = (CommData *) connectReq->handle->data;

                    auto commInterface = (CommTCP*)commData->getInterface();

                    auto target = commData->getComponent();

                    bool res = commData->getMsg()->writeToStream(target, onTCPSendCB);

                    if (!res) {

                        LOGS_E(commInterface->getHost(), "Write TCP is failed!!!");

                    }

                    free(connectReq);

                });

        if (result == UV_EMFILE) {

            std::this_thread::sleep_for(std::chrono::seconds(1));

            continue;
        }

        if (result != 0) {

            LOGS_E(getHost(), "Can not connect to server!!!, error : %s", uv_err_name(result));

        }

        break;
    }

    if (!tryCount) {

        LOGS_E(getHost(), "System still has too many open files!!!");
    }

    return tryCount != 0;
}

bool CommTCP::onUDPSend(const TypeComponentUnit &target, TypeMessage msg) {

    auto *client = (uv_udp_t *) calloc(1, sizeof(uv_udp_t));

    int result = uv_udp_init_ex(&consumeLoop, client, AF_INET);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init multicast socket!!!, err : %s", uv_err_name(result));

        return false;
    }

    result = uv_udp_set_multicast_interface(client, NetUtil::getIPString(address->get()).c_str());

    if (result != 0) {

        LOGS_E(getHost(), "Can not set multicast interface!!!, err : %s", uv_err_name(result));

        return false;
    }

    target->setHandle(client);

    bool res = msg->writeToStream(target, onUDPSendCB);

    if (!res) {

        LOGS_E(getHost(), "Write UDP is failed!!!");

    }

    return true;
}

bool CommTCP::onSend(const TypeComponentUnit &target, TypeMessage msg) {

    if (*target->getAddress() != *getMulticastAddress()) {

        return onTCPSend(target, std::move(msg));

    }

    return onUDPSend(target, std::move(msg));
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

            TypeAddress destAddress = std::make_shared<Address>(COMMINTERFACE_TCPIP,
                                                                getDevice()->getBase(), DEFAULT_TCP_PORT + i);

            if (*destAddress != *address) {

                list.push_back(destAddress);

            }

        }

    } else {

        uint32_t range = (1u << (32u - getDevice()->getMask())) - 2u;

        uint32_t mask = 0x80000000 >> (getDevice()->getMask() - 1u);

        uint32_t net = mask & getDevice()->getBase();

        uint32_t startIP = net + 1;

        for (uint32_t i = 0; i < range; i++) {

            if (startIP != address->get().base) {

                TypeAddress destAddress = std::make_shared<Address>(COMMINTERFACE_TCPIP, startIP, DEFAULT_TCP_PORT);

                list.push_back(destAddress);

            }

            startIP++;
        }

    }

    return list;
}
