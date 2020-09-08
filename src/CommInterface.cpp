//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterface.h"

CommInterface::CommInterface(const TypeHostUnit& _host, const TypeDevice& _device,
                             const InterfaceSchedulerCB *receiverCB)
        : host(_host), device(_device) {

    uv_loop_init(&produceLoop);

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

}

void CommInterface::end() {

//    char buf[1] = {SHUTDOWN_NOTIFIER};
//
//    write(notifierPipe[1], buf, 1);

    threadProduce.join();

    threadConsume.join();
}

bool CommInterface::initThread() {

    threadProduce = std::thread([](CommInterface *commInterface) {

        uv_loop_init(&commInterface->produceLoop);

        commInterface->initInterface();

        uv_run(&commInterface->produceLoop, UV_RUN_DEFAULT);

        uv_loop_close(&commInterface->produceLoop);

    }, this);

    threadConsume = std::thread([](CommInterface *commInterface) {

        uv_loop_init(&commInterface->consumeLoop);

        while(true) {

            if (!commInterface->scheduler->process()) {

                break;
            }

            uv_run(&commInterface->consumeLoop, UV_RUN_ONCE);

        }

        uv_loop_close(&commInterface->consumeLoop);

    }, this);

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
