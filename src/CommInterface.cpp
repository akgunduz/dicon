//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterface.h"

CommInterface::CommInterface(const TypeHostUnit& _host, const TypeDevice& _device,
                             const InterfaceSchedulerCB *receiveCB)
        : host(_host), device(_device) {

    uv_loop_init(&receiveLoop);

    scheduler = new Scheduler();

    senderCB = new InterfaceSchedulerCB([](void *arg, const TypeSchedulerItem& item) -> bool {

        bool status;
        auto *commInterface = (CommInterface *) arg;

        auto msgItem = std::static_pointer_cast<MessageItem>(item);

        auto msgType = msgItem->getMessage()->getHeader().getType();

        LOGC_T(commInterface->getHost(), msgItem->getUnit(), MSGDIR_SEND,
               "\"%s\" is sending",
               MessageType::getMsgName(msgType));

        auto target = std::make_shared<ComponentUnit>(msgItem->getUnit());

        if (msgItem->getUnit()->getAddress() != commInterface->getMulticastAddress()) {

            status = commInterface->runSender(target, std::move(msgItem->getMessage()));

        } else {

            status = commInterface->runMulticastSender(target, std::move(msgItem->getMessage()));
        }

        LOGC_D(commInterface->getHost(), msgItem->getUnit(), MSGDIR_SEND,
               "\"%s\" is sent",
               MessageType::getMsgName(msgType));

        return status;

    }, this);

    scheduler->setCB(MSGDIR_RECEIVE, receiveCB);
    scheduler->setCB(MSGDIR_SEND, senderCB);
}

void CommInterface::end() {

    char buf[1] = {SHUTDOWN_NOTIFIER};

    write(notifierPipe[1], buf, 1);

    threadRcv.join();

    uv_loop_close(&receiveLoop);
}

bool CommInterface::initThread() {

    if (pipe(notifierPipe) < 0) {
        LOGS_E(getHost(), "Notifier Pipe Init failed with err : %d!!!", errno);
        return false;
    }

    LOGS_T(getHost(), "Init Notifier PIPE OK!!!");

    threadRcv = std::thread([](CommInterface *commInterface) {

        return uv_run(&commInterface->receiveLoop, UV_RUN_DEFAULT);

    }, this);

    return true;
}

CommInterface::~CommInterface() {

    LOGP_T("Deallocating Interface");

    delete senderCB;

    delete scheduler;

    close(notifierPipe[1]);
    close(notifierPipe[0]);
}

bool CommInterface::push(MSG_DIR type, const TypeCommUnit& target, TypeMessage msg) {

    if (target->getAddress().getInterface() == getType()) {

        auto msgItem = std::make_shared<MessageItem>(type, target, std::move(msg));

        scheduler->push(std::move(msgItem));

        return true;
    }

    LOGS_E(getHost(), "Interface is not suitable for target : %d", target->getAddress().get().base);

    return false;
}

Address &CommInterface::getAddress() {

    return address;
}

Address &CommInterface::getMulticastAddress() {

    return multicastAddress;
}

void CommInterface::setAddress(Address &_address) {

    address = _address;
}

void CommInterface::setMulticastAddress(Address &_multicastAddress) {

    multicastAddress = _multicastAddress;
    multicastAddress.setMulticast(true);
}

const TypeHostUnit& CommInterface::getHost() {

    return host;
}

const TypeDevice& CommInterface::getDevice() {

    return device;
}
