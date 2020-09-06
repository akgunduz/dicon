//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommTCP.h"
#include "NetUtil.h"

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

    int result = uv_tcp_init(&receiveLoop, &tcpServer);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP socket!!!");

        return false;
    }

    tcpServer.data = this;

    int tryCount = TRY_COUNT;

    Address address(getDevice()->getBase(), lastFreeTCPPort);

    while (tryCount--) {

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByAddress(address);

        result = uv_tcp_bind(&tcpServer, (const struct sockaddr *) &serverAddress, 0);

        if (result < 0 || tcpServer.delayed_error != 0) {

            lastFreeTCPPort++;
            address.setPort(lastFreeTCPPort);
            continue;
        }

        break;
    }

    lastFreeTCPPort++;

    if (!tryCount) {

        LOGS_E(getHost(), "Could not bind to socket!!!");

        return false;
    }

    result = uv_listen(
            (uv_stream_t *) &tcpServer, 1000,

            [](uv_stream_t *serverPtr, int status) {

                auto commInterface = ((CommTCP *) serverPtr->data);

                if (status < 0) {
                    LOGS_E(commInterface->getHost(), "Socket listen with err : %d!!!", status);
                    return;
                }

                ((CommTCP *) serverPtr->data)->onConnection();

            });

    if (result != 0) {

        LOGS_E(getHost(), "Socket listen with err : %d!!!", result);

        uv_close((uv_handle_t *) &tcpServer, [](uv_handle_t *handle) {

            LOGP_I("TCP handle is closed!!!");
        });

        return false;
    }

    setAddress(address);

    LOGS_T(getHost(), "Using address : %s", NetUtil::getIPPortString(address.get()).c_str());

    return true;

}

bool CommTCP::initMulticast() {

    int tryCount = 10;

    Address address(MULTICAST_ADDRESS, lastFreeMulticastPort, true);

    int result = uv_udp_init(&receiveLoop, &multicastServer);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Multicast socket!!!");

        return false;
    }

    while (tryCount--) {

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByPort(lastFreeMulticastPort);

        result = uv_udp_bind(&multicastServer, (const struct sockaddr *) &serverAddress, UV_UDP_REUSEADDR);

        if (result < 0) {

            lastFreeMulticastPort++;
            address.setPort(lastFreeMulticastPort);
            continue;
        }

        break;
    }

    if (!tryCount) {

        LOGS_E(getHost(), "Could not bind to socket!!!");

        return false;
    }

    setMulticastAddress(address);

    uv_udp_set_membership(&multicastServer, NetUtil::getIPString(getMulticastAddress().get()).c_str(),
                          NetUtil::getIPString(getAddress().get()).c_str(), UV_JOIN_GROUP);

    multicastServer.data = new CommData{std::make_unique<Message>(getHost()),
                                        std::make_shared<ComponentUnit>(), this};

    uv_udp_recv_start(

            &multicastServer,

            [](uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {

                auto data = (CommData *) client->data;

                data->interface->onAlloc(suggested_size, buf);

            },

            [](uv_udp_t *client, ssize_t nRead, const uv_buf_t *buf,
               const struct sockaddr *addr, unsigned flags) {

                //LOGP_E("Data received, count : %d, bufPtr : %s",
                //      nRead, Util::hex2str((uint8_t *) buf->base, nRead).c_str());

                if (nRead == 0) {

                    return;
                }

                auto data = (CommData *) client->data;

                if (nRead == UV_ECONNRESET) {

                    data->interface->onFree(buf);

                    return;
                }

                data->interface->onRead(data->component,
                                        data->msg, (uint8_t *) buf->base, nRead);

            });

    LOGS_T(getHost(), "Using multicast address : %s", NetUtil::getIPPortString(getMulticastAddress().get()).c_str());

    return true;
}

bool CommTCP::onAlloc(size_t suggested_size, uv_buf_t *buf) {

    buf->base = (char *) malloc(suggested_size);
    assert(buf->base != nullptr);
    buf->len = suggested_size;

    return true;
}

bool CommTCP::onFree(const uv_buf_t *buf) {

    free(buf->base);

    return true;
}

bool CommTCP::onRead(TypeComponentUnit &component, TypeMessage &msg, const uint8_t *buf, size_t nRead) {

    bool isDone = msg->onRead(component, buf, nRead);

    if (isDone) {

        msg->build(component);

        auto owner = msg->getHeader().getOwner();

        push(MSGDIR_RECEIVE, owner, std::move(msg));
    }

    return isDone;
}

bool CommTCP::onWrite(const TypeComponentUnit &target, const uint8_t *buf, size_t size) {

    return true;
}

bool CommTCP::onConnection() {

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    int result = uv_tcp_init(&receiveLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP client socket!!!");

        return false;
    }

    result = uv_accept((uv_stream_t *) &tcpServer, (uv_stream_t *) client);

    if (result != 0) {

        LOGS_E(getHost(), "Socket accept with err : %d!!!", result);

        uv_close((uv_handle_t *) &client, [](uv_handle_t *handle) {

            free(handle);

        });

        return false;
    }

    client->data = new CommData{std::make_unique<Message>(getHost()),
            std::make_shared<ComponentUnit>(), this};

    result = uv_read_start(

            (uv_stream_t *) client,

            [](uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {

                auto data = (CommData *) client->data;

                data->interface->onAlloc(suggested_size, buf);

            },

            [](uv_stream_t *client, ssize_t nRead, const uv_buf_t *buf) {

                if (nRead == 0) {

                    return;
                }

                auto data = (CommData *) client->data;

                if (nRead == UV_EOF || nRead == UV_ECONNRESET) {

                    data->interface->onShutdown(client);

                    data->interface->onFree(buf);

                    return;
                }

                data->interface->onRead(data->component,
                                        data->msg, (uint8_t *) buf->base, nRead);
            });

    if (result != 0) {

        LOGS_E(getHost(), "Read start with err : %d!!!", result);

        uv_close((uv_handle_t *) &client, [](uv_handle_t *handle) {

            free(handle);

        });

        return false;
    }

    return true;
}

bool CommTCP::onShutdown(uv_stream_t* client) {

    auto *shutdown_req = (uv_shutdown_t *) malloc(sizeof(uv_shutdown_t));

    uv_shutdown(shutdown_req, client, [](uv_shutdown_t *req, int status) {

        if (status) {

            LOGP_E("Shutdown problem : %d!!!", status);
        }

        free(req->handle->data);
        free(req->handle);
        free(req);
    });

    return false;
}

bool CommTCP::runSender(const TypeComponentUnit &target, TypeMessage msg) {

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    int result = uv_tcp_init(scheduler->getLoop(), client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP client socket!!!");

        return false;
    }

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    client->data = new CommData{std::move(msg), target, this};

    auto *connectReq = (uv_connect_t *) malloc(sizeof(uv_connect_t));

    result = uv_tcp_connect(

            connectReq, client, (const struct sockaddr *) &clientAddress,

            [](uv_connect_t *connectReq, int status) {

                if (status) {

                    LOGP_E("TCP Connect problem, error : %d!!!", status);

                    uv_close((uv_handle_t *) &connectReq->handle,
                             [](uv_handle_t *handle) {

                        free(handle);

                    });

                    return;
                }

                auto data = (CommData *) connectReq->handle->data;

                data->component->setHandle(connectReq->handle);

                free(connectReq);

                bool res = data->msg->writeToStream(

                        data->component,

                        [](const TypeComponentUnit &target, const uint8_t *buf, size_t size) -> bool {

                            uv_buf_t bufPtr = uv_buf_init((char *) buf, size);

                            auto *writeReq = (uv_write_t *) malloc(sizeof(uv_write_t));

                            int result = uv_write(

                                    writeReq, target->getHandle(), &bufPtr, 1,

                                    [](uv_write_t *writeReq, int status) {

                                        free(writeReq);

                                        if (status) {
                                            LOGP_E("Write request problem, error : %d!!!", status);
                                        }
                                    });

                            if (result != 0) {

                                LOGP_E("Write problem, error : %d!!!", result);
                                return false;
                            }

                            return true;
                        });

                if (!res) {

                    LOGP_E("Write stream is failed!!!");
                }

            });

    if (result != 0) {

        LOGS_E(getHost(), "Can not connect to server!!!, error : %d", result);

        uv_close((uv_handle_t *) &client, [](uv_handle_t *handle) {

            free(handle);

        });

        return false;
    }

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


    msg->writeToStream(

            target,

            [](const TypeComponentUnit &target, const uint8_t *buf,
               size_t size) -> bool {

                uv_buf_t bufPtr = uv_buf_init((char *) buf, size);

                auto *writeReq = (uv_write_t *) malloc(sizeof(uv_write_t));
                uv_write(writeReq, target->getHandle(), &bufPtr, 1,
                         [](uv_write_t *req, int status) {

                             if (status) {
                                 LOGP_E("Write request problem, error : %d!!!", status);
                             }

                             free(req);
                         });

                return true;
            });

//    shutdown(clientSocket, SHUT_RDWR);
//
//    close(clientSocket);

    return true;
}

CommTCP::~CommTCP() {

    LOGP_T("Deallocating Net");

    end();

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

        uint32_t range = (1u << (32u - getDevice()->getMask())) - 2u;

        uint32_t mask = 0x80000000 >> (getDevice()->getMask() - 1u);

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
