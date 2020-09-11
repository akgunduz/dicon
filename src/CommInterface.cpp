//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterface.h"

CommInterface::CommInterface(const TypeHostUnit& _host, const TypeDevice& _device,
                             const InterfaceSchedulerCB *receiverCB)
        : host(_host), device(_device) {

    scheduler = new Scheduler();

    senderCB = new InterfaceSchedulerCB([](void *arg, const TypeSchedulerItem& item) -> bool {

        return ((CommInterface*) arg)->send(item);

    }, this);

    scheduler->setCB(MSGDIR_RECEIVE, receiverCB);
    scheduler->setCB(MSGDIR_SEND, senderCB);
}

CommInterface::~CommInterface() {

    LOGP_T("Deallocating Interface");

    delete senderCB;

    delete scheduler;

    close(notifierPipe[0]);
    close(notifierPipe[1]);

}

void CommInterface::end() {

    char buf[1] = {SHUTDOWN_NOTIFIER};

    write(notifierPipe[1], buf, 1);

    scheduler->end();

    threadProduce.join();

    threadConsume.join();
}

bool CommInterface::initThread() {

    if (pipe(notifierPipe) < 0) {

        LOGS_E(getHost(), "Notifier Pipe Init failed with err : %d!!!", errno);

        return false;
    }

    LOGS_T(getHost(), "Init Notifier PIPE OK!!!");

    threadProduce = std::thread([](const TypeInterface& commInterface) {

//        commInterface->initInterface();
//
//        commInterface->runReceiver();

    }, shared_from_this());

    threadConsume = std::thread([](const TypeInterface& commInterface) {

        while(true) {

            if (!commInterface->scheduler->process()) {

                break;
            }

        }

    }, shared_from_this());

    return true;
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

const TypeHostUnit& CommInterface::getHost() {

    return host;
}

const TypeDevice& CommInterface::getDevice() {

    return device;
}
