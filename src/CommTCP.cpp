//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommTCP.h"
#include "NetUtil.h"
#include "CommData.h"

CommTCP::CommTCP(const TypeHostUnit &host, const TypeDevice &device, const InterfaceSchedulerCB *receiverCB)
        : CommInterface(host, device, receiverCB) {

}

CommTCP::~CommTCP() {

    LOGP_T("Deallocating TCP Interface");

    end();
}

bool CommTCP::initInterface() {

    return initTCP() && initMulticast();
}

bool CommTCP::initTCP() {

    int result = uv_tcp_init(&produceLoop, &tcpServer);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP socket!!!");

        return false;
    }

    tcpServer.data = this;

    int tryCount = TRY_COUNT;

    address.set(getDevice()->getBase(), lastFreeTCPPort);

    while (tryCount--) {

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByAddress(address);

        result = uv_tcp_bind(&tcpServer, (const struct sockaddr *) &serverAddress, 0);

        if (result < 0 || tcpServer.delayed_error != 0) {

            address.setPort(++lastFreeTCPPort);

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
                    LOGS_E(commInterface->getHost(), "Socket listen with err : %s", uv_err_name(status));
                    return;
                }

                ((CommTCP *) serverPtr->data)->onServerConnect();

            });

    if (result != 0) {

        LOGS_E(getHost(), "Socket listen with err : %s", uv_err_name(result));

        uv_close((uv_handle_t *) &tcpServer, onClose);

        return false;
    }

    LOGS_T(getHost(), "Using address : %s", NetUtil::getIPPortString(getAddress().get()).c_str());

    return true;
}

bool CommTCP::initMulticast() {

    int result = uv_udp_init(&produceLoop, &multicastServer);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Multicast socket!!!");

        return false;
    }

    int tryCount = TRY_COUNT;

    multicastAddress.set(MULTICAST_ADDRESS, lastFreeMulticastPort, true);

    while (tryCount--) {

        struct sockaddr_in serverAddress = NetUtil::getInetAddressByPort(lastFreeMulticastPort);

        result = uv_udp_bind(&multicastServer, (const struct sockaddr *) &serverAddress, UV_UDP_REUSEADDR);

        if (result < 0) {

            multicastAddress.setPort(++lastFreeMulticastPort);

            continue;
        }

        break;
    }

    if (!tryCount) {

        LOGS_E(getHost(), "Could not bind to socket!!!");

        uv_close((uv_handle_t*) &multicastServer, onClose);

        return false;
    }

    result = uv_udp_set_membership(&multicastServer, NetUtil::getIPString(getMulticastAddress().get()).c_str(),
                          NetUtil::getIPString(getAddress().get()).c_str(), UV_JOIN_GROUP);

    if (result != 0) {

        LOGS_E(getHost(), "Can not join to multicast group!!!");

        uv_close((uv_handle_t*) &multicastServer, onClose);

        return false;
    }

    multicastServer.data = new CommData(shared_from_this());

    result = uv_udp_recv_start(&multicastServer,

            [](uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {

                onAlloc(buf, suggested_size);

            },

            [](uv_udp_t *client, ssize_t nRead, const uv_buf_t *buf,
               const struct sockaddr *addr, unsigned flags) {

                if (nRead == 0) {

                    return;
                }

                if (nRead == UV_ECONNRESET) {

                    onFree(buf);

                    return;
                }

                auto commData = (CommData *) client->data;

                auto commInterface = std::static_pointer_cast<CommTCP>(commData->getInterface());

                bool isDone = commInterface->onRead(commData->getComponent(),
                                                    commData->getMsg(), (uint8_t *) buf->base, nRead);

                if (isDone) {

                    commData->reInitialize();
                }
            });

    if (result != 0) {

        LOGS_E(getHost(), "Can not start receiving UDP data!!!");

        uv_close((uv_handle_t*) &multicastServer, onClose);

        return false;
    }

    LOGS_T(getHost(), "Using multicast address : %s", NetUtil::getIPPortString(getMulticastAddress().get()).c_str());

    return true;
}

bool CommTCP::onAlloc(uv_buf_t *buf, size_t size, const uint8_t* copy) {

    buf->base = (char *) malloc(size);

    assert(buf->base != nullptr);

    buf->len = size;

    if (copy) {

        memcpy(buf->base, copy, size);
    }

    //LOGP_E("Allocated Buffer, Pointer : %p, Len : %d !!!", buf, size);

    return true;
}

bool CommTCP::onFree(const uv_buf_t *buf) {

    //LOGP_E("DeAllocating Buffer, Pointer : %p", buf->base);

    free(buf->base);

    return true;
}

void CommTCP::onClose(uv_handle_t* handle) {

    LOGP_I("Handle is closed!!!");

    free(handle);
}

bool CommTCP::onShutdown(uv_stream_t *client) {

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

bool CommTCP::onRead(const TypeComponentUnit &component, TypeMessage &msg, const uint8_t *buf, size_t nRead) {

    bool isDone = msg->onRead(component, buf, nRead);

    if (isDone) {

        msg->build(component);

        auto owner = msg->getHeader().getOwner();

        push(MSGDIR_RECEIVE, owner, std::move(msg));
    }

    return isDone;
}

bool CommTCP::onTCPWrite(const TypeComponentUnit &target, const uint8_t *buffer, size_t size) {

    uv_buf_t bufPtr = uv_buf_init((char *) buffer, size);

    auto *writeReq = (uv_write_t *) malloc(sizeof(uv_write_t));

    int result = uv_write(

            writeReq, (uv_stream_t *) target->getHandle(), &bufPtr, 1,

            [](uv_write_t *writeReq, int status) {

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

bool CommTCP::onMulticastWrite(const TypeComponentUnit &target, const uint8_t *buffer, size_t size) {

    uv_buf_t bufPtr;
    onAlloc(&bufPtr, size, buffer);

    auto *writeReq = (uv_udp_send_t *) malloc(sizeof(uv_udp_send_t));

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    int result = uv_udp_send(writeReq, (uv_udp_t *) target->getHandle(), &bufPtr,

        1, (const struct sockaddr *) &clientAddress,

         [](uv_udp_send_t *writeReq, int status) {

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

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    int result = uv_tcp_init(&produceLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP client socket!!!");

        return false;
    }

    result = uv_accept((uv_stream_t *) &tcpServer, (uv_stream_t *) client);

    if (result != 0) {

        LOGS_E(getHost(), "Socket accept with err : %d!!!", result);

        uv_close((uv_handle_t *) &client, onClose);

        return false;
    }

    client->data = new CommData(shared_from_this());

    result = uv_read_start(

            (uv_stream_t *) client,

            [](uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {

                onAlloc(buf, suggested_size);

            },

            [](uv_stream_t *client, ssize_t nRead, const uv_buf_t *buf) {

                if (nRead == 0) {

                    onFree(buf);

                    return;
                }

                if (nRead == UV_EOF || nRead == UV_ECONNRESET) {

                    onShutdown(client);

                    onFree(buf);

                    return;
                }

                auto commData = (CommData *) client->data;

                auto commInterface = std::static_pointer_cast<CommTCP>(commData->getInterface());

                bool isDone = commInterface->onRead(commData->getComponent(),
                                                    commData->getMsg(), (uint8_t *) buf->base, nRead);

                if (isDone) {

                    commData->reInitialize();
                }

                onFree(buf);
            });

    if (result != 0) {

        LOGS_E(getHost(), "Read start with err : %s!!!", uv_err_name(result));

        uv_close((uv_handle_t *) &client, onClose);

        return false;
    }

    return true;
}

bool CommTCP::onClientConnect(const TypeComponentUnit &target, TypeMessage &msg, uv_stream_t *handle) {

    target->setHandle(handle);

    bool res = msg->writeToStream(target, onTCPWrite);

    if (!res) {

        LOGP_E("Write stream is failed!!!");

        return false;
    }

    return true;
}

bool CommTCP::runSender(const TypeComponentUnit& target, TypeMessage msg) {

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    int result = uv_tcp_init(&consumeLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init TCP client socket!!!");

        return false;
    }

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    client->data = new CommData(shared_from_this(), target, msg);

    auto *connectReq = (uv_connect_t *) malloc(sizeof(uv_connect_t));

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

                    auto commInterface = std::static_pointer_cast<CommTCP>(commData->getInterface());

                    commInterface->onClientConnect(commData->getComponent(), commData->getMsg(), connectReq->handle);

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

bool CommTCP::runMulticastSender(const TypeComponentUnit &target, TypeMessage msg) {

    auto *client = (uv_udp_t *) malloc(sizeof(uv_udp_t));

    int result = uv_udp_init_ex(&consumeLoop, client, AF_INET);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init multicast socket!!!, err : %s", uv_err_name(result));

        return false;
    }

    result = uv_udp_set_multicast_interface(client, NetUtil::getIPString(getAddress().get()).c_str());

    if (result != 0) {

        LOGS_E(getHost(), "Can not set multicast interface!!!, err : %s", uv_err_name(result));

        return false;
    }

    target->setHandle(client);

    msg->writeToStream(target, onMulticastWrite);

    return true;
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
