//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommPipe.h"
#include "NetUtil.h"
#include "CommData.h"
#include "Util.h"

CommPipe::CommPipe(TypeHostUnit host, const TypeDevice &device, const CommInterfaceCB *receiverCB)
        : CommInterface(std::move(host), device, receiverCB) {

}

CommPipe::~CommPipe() {

    LOGS_T(getHost(), "Deallocating Pipe Interface");

}

bool CommPipe::initInterface() {

    LOGS_T(getHost(), "Initializing Pipe Interface");

    return initPipe();
}

bool CommPipe::initPipe() {

    pipeServer = (uv_pipe_t*) malloc(sizeof(uv_pipe_t));

    int result = uv_pipe_init(&produceLoop, pipeServer, false);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Pipe socket!!!");

        return false;
    }

    pipeServer->data = new CommData(this);

    int tryCount = TRY_COUNT;

    address.set(getDevice()->getBase(), lastFreePipePort);

    while (tryCount--) {

        struct sockaddr_un serverAddress = NetUtil::getUnixAddress(address);

        result = uv_pipe_bind(pipeServer, serverAddress.sun_path);

        if (result < 0 || pipeServer->delayed_error != 0) {

            address.setPort(++lastFreePipePort);

            continue;
        }

        break;
    }

    lastFreePipePort++;

    if (!tryCount) {

        LOGS_E(getHost(), "Could not bind to socket!!!");

        return false;
    }

    result = uv_listen((uv_stream_t *) pipeServer, 1000,

            [](uv_stream_t *server, int status) {

                auto commData = (CommData *) server->data;

                   if (status < 0) {
                    LOGS_E(commData->getInterface()->getHost(), "Socket listen with err : %s", uv_err_name(status));
                    return;
                }

                ((CommPipe *) commData->getInterface())->onServerConnect();

            });

    if (result != 0) {

        LOGS_E(getHost(), "Socket listen with err : %s", uv_err_name(result));

        onClose((uv_handle_t *)pipeServer);

        return false;
    }

    LOGS_T(getHost(), "Using address : %s", NetUtil::getIPPortString(getAddress().get()).c_str());

    return true;
}

bool CommPipe::onReceive(uv_handle_t* client, ssize_t nRead, const uv_buf_t *buf) {

    if (nRead == 0) {

        onFree(buf);

        return false;
    }

    if (nRead == UV_EOF || nRead == UV_ECONNRESET) {

        onClose((uv_handle_t*)client);

        onFree(buf);

        return true;
    }

    auto commData = (CommData *) client->data;

    auto commInterface = (CommPipe*)commData->getInterface();

    STATUS status = commInterface->onRead(commData->getComponent(),
                                          commData->getMsg(), (uint8_t *) buf->base, nRead);

    if (status == STATUS_DONE) {

        commData->reInitialize();

    } else if (status == STATUS_SHUTDOWN) {

        commInterface->shutdown();
    }

    onFree(buf);

    return false;
}

bool CommPipe::onSendCB(const TypeComponentUnit &target, const uint8_t *buffer, size_t size) {

    uv_buf_t bufPtr = uv_buf_init((char *) buffer, size);

    auto *writeReq = (uv_write_t *) malloc(sizeof(uv_write_t));

    int result = uv_write(

            writeReq, (uv_stream_t *) target->getHandle(), &bufPtr, 1,

            [](uv_write_t *writeReq, int status) {

                free(writeReq);

                if (status) {
                    LOGP_E("Pipe Write request problem, error : %d!!!", status);
                }
            });

    if (result != 0) {

        LOGP_E("Pipe Write problem, error : %d!!!", result);

        return false;
    }

    return true;
}

bool CommPipe::onServerConnect() {

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    int result = uv_tcp_init(&produceLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Pipe client socket!!!");

        return false;
    }

    result = uv_accept((uv_stream_t *) pipeServer, (uv_stream_t *) client);

    if (result != 0) {

        LOGS_E(getHost(), "Socket accept with err : %d!!!", result);

        onClose((uv_handle_t *)&client);

        return false;
    }

    client->data = new CommData(this);

    result = uv_read_start((uv_stream_t *) client, onAlloc,

            [](uv_stream_t *client, ssize_t nRead, const uv_buf_t *buf) {

                onReceive((uv_handle_t*)client, nRead, buf);
            });

    if (result != 0) {

        LOGS_E(getHost(), "Read start with err : %s!!!", uv_err_name(result));

        onClose((uv_handle_t *)&client);

        return false;
    }

    return true;
}

bool CommPipe::onSend(const TypeComponentUnit& target, TypeMessage msg) {

    auto *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));

    int result = uv_tcp_init(&consumeLoop, client);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Pipe client socket!!!");

        return false;
    }

    target->setHandle(client);

    client->data = new CommData(this, target, msg);

    sockaddr_in clientAddress = NetUtil::getInetAddressByAddress(target->getAddress());

    auto *connectReq = (uv_connect_t *) malloc(sizeof(uv_connect_t));

    int tryCount = TRY_COUNT;

    while (tryCount--) {

        result = uv_tcp_connect(

                connectReq, client, (const struct sockaddr *) &clientAddress,

                [](uv_connect_t *connectReq, int status) {

                    if (status) {

                        LOGP_E("Pipe Connect problem, error : %s!!!", uv_strerror(status));

                        return;
                    }

                    auto commData = (CommData *) connectReq->handle->data;

                    auto commInterface = (CommPipe*)commData->getInterface();

                    auto target = commData->getComponent();

                    bool res = commData->getMsg()->writeToStream(target, onSendCB);

                    if (!res) {

                        LOGS_E(commInterface->getHost(), "Write Pipe is failed!!!");

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

COMM_INTERFACE CommPipe::getType() {

    return COMMINTERFACE_UNIXSOCKET;
}

bool CommPipe::isSupportMulticast() {

    return false;
}

TypeAddressList CommPipe::getAddressList() {

    TypeAddressList list;

    for (const auto& entry : std::filesystem::directory_iterator(Util::tmpPath)) {

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
