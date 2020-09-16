//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterface.h"
#include "CommData.h"

CommInterface::CommInterface(const TypeHostUnit &_host, const TypeDevice &_device,
                             const InterfaceSchedulerCB *receiverCB)
        : host(_host), device(_device) {

    scheduler = new Scheduler();

    senderCB = new InterfaceSchedulerCB([](void *arg, const TypeSchedulerItem &item) -> bool {

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

        uv_walk(&commInterface->consumeLoop, [](uv_handle_t *handle, void *arg) {

            if (!uv_is_closing(handle)) {
                onClose(handle);
            }

        }, commInterface);

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

//    char buf[1] = {SHUTDOWN_NOTIFIER};
//
//    write(notifierPipe[1], buf, 1);

    onShutdown();

    scheduler->shutdown();

    uv_walk(&produceLoop, [](uv_handle_t *handle, void *arg) {

        if (!uv_is_closing(handle)) {
            onClose(handle);
        }

    }, this);
}

void CommInterface::onClose(uv_handle_t* handle) {

    if (!handle) {
        return;
    }

    uv_close(handle, [] (uv_handle_t* _handle) {

      //  LOGP_I("Handle is closed!!!");

        if (_handle->data != nullptr) {

            delete (CommData *) _handle->data;
        }

        free(_handle);
    });

}

bool CommInterface::push(MSG_DIR type, const TypeCommUnit &target, TypeMessage msg) {

    if (target->getAddress().getInterface() == getType()) {

        auto msgItem = std::make_shared<MessageItem>(type, target, std::move(msg));

        scheduler->push(msgItem);

        return true;
    }

    LOGS_E(getHost(), "Interface is not suitable for target : %d", target->getAddress().get().base);

    return false;
}

bool CommInterface::send(const TypeSchedulerItem &item) {

    bool status;

    auto msgItem = std::static_pointer_cast<MessageItem>(item);

    auto msgType = msgItem->getMessage()->getHeader().getType();

    LOGC_T(getHost(), msgItem->getUnit(), MSGDIR_SEND,
           "\"%s\" is sending",
           MessageType::getMsgName(msgType));

    auto target = std::make_shared<ComponentUnit>(msgItem->getUnit());

    if (msgItem->getUnit()->getAddress() != getMulticastAddress()) {

        status = runSender(target, std::move(msgItem->getMessage()));

    } else {

        status = runMulticastSender(target, std::move(msgItem->getMessage()));
    }

    LOGC_D(getHost(), msgItem->getUnit(), MSGDIR_SEND,
           "\"%s\" is sent",
           MessageType::getMsgName(msgType));

    return status;
}

Address &CommInterface::getAddress() {

    return address;
}

Address &CommInterface::getMulticastAddress() {

    return multicastAddress;
}

const TypeHostUnit &CommInterface::getHost() {

    return host;
}

const TypeDevice &CommInterface::getDevice() {

    return device;
}
