//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterface.h"
#include "Log.h"
#include "UvUtil.h"
#include "MessageItem.h"

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

            uv_run(&commInterface->consumeLoop, UV_RUN_DEFAULT);

        }

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

    UvUtil::onCloseAll(&produceLoop);
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

    if (target->getAddress()->getInterface() != getType()) {

        LOGC_T(getHost(), target, type, "Target Interface[%s] does not match with Host Interface[%s]",
               CommInterfaceType::getName(target->getAddress()->getInterface()),
               CommInterfaceType::getName(getType()));

        return false;
    }

    if (getHost()->getType() != target->getType() &&
            getHost()->getAddress(target->getType())->getInterface() != getType()) {

        LOGC_T(getHost(), target, type, "Configured Interface[%s] does not match with Host Interface[%s]",
               CommInterfaceType::getName(getHost()->getAddress(target->getType())->getInterface()),
               CommInterfaceType::getName(getType()));

        return false;
    }

    auto msgItem = std::make_shared<MessageItem>(type, target, std::move(msg));

    return scheduler->push(msgItem);
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
