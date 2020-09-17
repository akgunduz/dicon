//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterface.h"

#include <utility>
#include "CommData.h"

CommInterface::CommInterface(TypeHostUnit _host, const TypeDevice& _device,
                             const CommInterfaceCB *receiverCB)
        : host(std::move(_host)), device(_device) {

    address = std::make_shared<Address>();
    multicastAddress = std::make_shared<Address>();

    scheduler = new Scheduler();

    senderCB = new CommInterfaceCB([](void *arg, const TypeSchedulerItem& item) -> bool {

        return ((CommInterface *) arg)->send(item);

    }, this);

    scheduler->setCB(MSGDIR_RECEIVE, receiverCB);
    scheduler->setCB(MSGDIR_SEND, senderCB);
}

CommInterface::~CommInterface() {

    LOGP_T("Deallocating Interface");

    delete senderCB;

    delete scheduler;

}

bool CommInterface::initThread() {

    threadProduce = std::thread([](CommInterface *commInterface) {

        int status = uv_loop_init(&commInterface->produceLoop);

        if (status != 0) {

            LOGS_E(commInterface->getHost(), "Produce Loop can not created, returning, status : %s !!!",
                   uv_err_name(status));

            return false;
        }

        commInterface->initInterface();

        uv_run(&commInterface->produceLoop, UV_RUN_DEFAULT);

        status = uv_loop_close(&commInterface->produceLoop);

        if (status != 0) {

            LOGS_E(commInterface->getHost(), "Produce Loop does not gracefully closed, status : %s, returning!!!",
                   uv_err_name(status));

            return false;
        }

        LOGS_T(commInterface->getHost(), "Produce Loop is Done, returning!!!");

        return true;

    }, this);

    threadConsume = std::thread([](CommInterface *commInterface) {

        int status = uv_loop_init(&commInterface->consumeLoop);

        if (status != 0) {

            LOGS_E(commInterface->getHost(), "Consume Loop can not created, status : %s returning!!!",
                   uv_err_name(status));

            return false;
        }

        while (true) {

            if (!commInterface->scheduler->process()) {

                break;
            }

            uv_run(&commInterface->consumeLoop, UV_RUN_ONCE);

        }

        commInterface->onCloseAll(&commInterface->consumeLoop);

        uv_run(&commInterface->consumeLoop, UV_RUN_ONCE);

        status = uv_loop_close(&commInterface->consumeLoop);

        if (status != 0) {

            LOGS_E(commInterface->getHost(), "Consume Loop does not gracefully closed, status : %s returning!!!",
                   uv_err_name(status));

            return false;
        }

        LOGS_T(commInterface->getHost(), "Consume Loop is Done, returning!!!");

        return true;

    }, this);

    return true;
}

bool CommInterface::waitThread() {

    threadProduce.join();

    threadConsume.join();

    return true;
}

void CommInterface::shutdown() {

    scheduler->shutdown();

    onCloseAll(&produceLoop);
}

void CommInterface::onAlloc(uv_handle_t* handle, size_t size, uv_buf_t* buf) {

    buf->base = (char *) malloc(size);

    assert(buf->base != nullptr);

    buf->len = size;

    //LOGP_E("%s => Allocated Buffer, Pointer : %p, Len : %d !!!", id, buf->base, size);

}

void CommInterface::onFree(const uv_buf_t *buf) {

    //LOGP_E("DeAllocating Buffer, Pointer : %p", buf->base);

    free(buf->base);

}

void CommInterface::onClose(uv_handle_t* handle) {

    if (!handle) {
        return;
    }

    uv_close(handle, [] (uv_handle_t* _handle) {

        if (_handle->data != nullptr) {

            delete (CommData *) _handle->data;
        }

        free(_handle);
    });

}

void CommInterface::onCloseAll(uv_loop_t *loop) {

    uv_walk(loop, [](uv_handle_t *handle, void *arg) {

        if (!uv_is_closing(handle)) {
            onClose(handle);
        }

    }, this);
}

STATUS CommInterface::onRead(const TypeComponentUnit &component, TypeMessage &msg,
                             const uint8_t *buf, size_t nRead) {

    bool isDone = msg->onRead(component, buf, nRead);

    if (isDone) {

        msg->build(component);

        auto owner = msg->getHeader().getOwner();

        if (msg->getHeader().getType() == MSGTYPE_SHUTDOWN) {

            return STATUS_SHUTDOWN;
        }

        push(MSGDIR_RECEIVE, owner, std::move(msg));

        return STATUS_DONE;
    }

    return STATUS_PROGRESS;
}

bool CommInterface::push(MSG_DIR type, const TypeCommUnit &target, TypeMessage msg) {

    if (getHost()->getAddress(target->getType())->getInterface() == getType()) {

        auto msgItem = std::make_shared<MessageItem>(type, target, std::move(msg));

        return scheduler->push(msgItem);
    }

    LOGC_T(getHost(), target, type, "Configured Interface[%s] does not match with ongoing Interface[%s]",
           CommInterfaceType::getName(getHost()->getAddress(target->getType())->getInterface()),
           CommInterfaceType::getName(getType()));

    return false;
}

bool CommInterface::send(const TypeSchedulerItem &item) {

    auto msgItem = std::static_pointer_cast<MessageItem>(item);

    auto msgType = msgItem->getMessage()->getHeader().getType();

    LOGC_T(getHost(), msgItem->getUnit(), MSGDIR_SEND,
           "\"%s\" is sending",
           MessageType::getMsgName(msgType));

    auto target = std::make_shared<ComponentUnit>(msgItem->getUnit());

    bool status = onSend(target, std::move(msgItem->getMessage()));

    LOGC_D(getHost(), target, MSGDIR_SEND,
           "\"%s\" is sent",
           MessageType::getMsgName(msgType));

    return status;
}

TypeAddress& CommInterface::getAddress() {

    return address;
}

TypeAddress& CommInterface::getMulticastAddress() {

    return multicastAddress;
}

const TypeHostUnit& CommInterface::getHost() {

    return host;
}

const TypeDevice& CommInterface::getDevice() {

    return device;
}
