//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommPipe.h"
#include "UtilNet.h"
#include "UtilUV.h"
#include "Util.h"
#include "CommData.h"
#include "Log.h"

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

    pipeServer = (uv_pipe_t*) calloc(1, sizeof(uv_pipe_t));

    int result = uv_pipe_init(&produceLoop, pipeServer, false);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Pipe socket!!!");

        return false;
    }

    pipeServer->data = new CommData(this);

    int tryCount = TRY_COUNT;

    address->set(COMMINTERFACE_UNIXSOCKET, getDevice()->getBase(), lastFreePipePort);

    while (tryCount--) {

        sockaddr_un serverAddress = UtilNet::getUnixAddress(address);

        result = uv_pipe_bind(pipeServer, serverAddress.sun_path);

        if (result < 0) {

            address->setPort(++lastFreePipePort);

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

        UtilUV::onClose((uv_handle_t *)pipeServer);

        return false;
    }

    LOGS_T(getHost(), "Using address : %s", UtilNet::getIPPortString(address->get()).c_str());

    return true;
}

bool CommPipe::onReceive(uv_handle_t* client, ssize_t nRead, const uv_buf_t *buf) {

    if (nRead == 0) {

        UtilUV::onFree(buf);

        return false;
    }

    if (nRead == UV_EOF || nRead == UV_ECONNRESET) {

        UtilUV::onClose((uv_handle_t*)client);

        UtilUV::onFree(buf);

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

    UtilUV::onFree(buf);

    return false;
}

bool CommPipe::onSendCB(const TypeComponentUnit &target, const uint8_t *buffer, size_t size) {

    uv_buf_t bufPtr = uv_buf_init((char *) buffer, (unsigned int)size);

    auto *writeReq = (uv_write_t *) calloc(1, sizeof(uv_write_t));

    int result = uv_write(

            writeReq, (uv_stream_t *) target->getHandle(), &bufPtr, 1,

            [](uv_write_t *writeReq, int status) {

                if (status) {
                    LOGP_E("Pipe Write request problem, error : %d!!!", status);
                }

                UtilUV::onShutdown((uv_stream_t *)writeReq->handle);

                free(writeReq);

            });

    if (result != 0) {

        LOGP_E("Pipe Write problem, error : %d!!!", result);

        return false;
    }

    return true;
}

bool CommPipe::onServerConnect() {

    auto *client = (uv_pipe_t *) calloc(1, sizeof(uv_pipe_t));

    int result = uv_pipe_init(&produceLoop, client, false);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Pipe client socket!!!");

        return false;
    }

    result = uv_accept((uv_stream_t *) pipeServer, (uv_stream_t *) client);

    if (result != 0) {

        LOGS_E(getHost(), "Socket accept with err : %d!!!", result);

        UtilUV::onClose((uv_handle_t *)&client);

        return false;
    }

    client->data = new CommData(this);

    result = uv_read_start((uv_stream_t *) client, UtilUV::onAlloc,

                           [](uv_stream_t *client, ssize_t nRead, const uv_buf_t *buf) {

                onReceive((uv_handle_t*)client, nRead, buf);
            });

    if (result != 0) {

        LOGS_E(getHost(), "Read start with err : %s!!!", uv_err_name(result));

        UtilUV::onClose((uv_handle_t *)&client);

        return false;
    }

    return true;
}

bool CommPipe::onSend(const TypeComponentUnit& target, TypeMessage msg) {

    auto *client = (uv_pipe_t *) calloc(1, sizeof(uv_pipe_t));

    int result = uv_pipe_init(&consumeLoop, client, false);

    if (result != 0) {

        LOGS_E(getHost(), "Could not init Pipe client socket!!!");

        return false;
    }

    target->setHandle(client);

    client->data = new CommData(this, target, msg);

    sockaddr_un clientAddress = UtilNet::getUnixAddress(target->getAddress());

    auto *connectReq = (uv_connect_t *) calloc(1, sizeof(uv_connect_t));

    uv_pipe_connect(

        connectReq, client, clientAddress.sun_path,

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

    return true;
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

        if (!entry.is_socket() ||
                entry.path().filename().string().find(UNIXSOCKET_FILE_PREFIX) == std::string::npos) {
            continue;
        }

        BaseAddress base;

        uint32_t start = (uint32_t) entry.path().filename().string().find('_') + 1;

        uint32_t end = (uint32_t) entry.path().filename().string().find('.');

        std::string sAddress = entry.path().filename().string().substr(start, end - start);

        std::sscanf(sAddress.c_str(), "%u_%hu", &base.base, &base.port);

        TypeAddress found = std::make_shared<Address>(COMMINTERFACE_UNIXSOCKET, base.base, base.port);

        if (*address != *found) {
            list.push_back(found);
        }
    }

    return list;
}
