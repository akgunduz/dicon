//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Interface.h"

Interface::Interface(const TypeHostUnit& _host, const TypeDevice& _device, const InterfaceSchedulerCB *receiveCB)
        : host(_host), device(_device) {

    scheduler = new Scheduler();

    schedulerCB = new InterfaceSchedulerCB([](void *arg, const TypeSchedulerItem& item) -> bool {

        bool status;
        auto *interface = (Interface *) arg;

        auto msgItem = std::static_pointer_cast<MessageItem>(item);

        auto msgType = msgItem->getMessage()->getHeader().getType();

        LOGC_T(interface->getHost(), msgItem->getUnit(), MSGDIR_SEND,
               "\"%s\" is sending",
               MessageType::getMsgName(msgType));

        auto target = std::make_shared<ComponentUnit>(msgItem->getUnit());

        if (msgItem->getUnit()->getAddress() != interface->getMulticastAddress()) {

            status = interface->runSender(target, std::move(msgItem->getMessage()));

        } else {

            status = interface->runMulticastSender(target, std::move(msgItem->getMessage()));
        }

        LOGC_D(interface->getHost(), msgItem->getUnit(), MSGDIR_SEND,
               "\"%s\" is sent",
               MessageType::getMsgName(msgType));

        return status;

    }, this);

    scheduler->setCB(MSGDIR_RECEIVE, receiveCB);
    scheduler->setCB(MSGDIR_SEND, schedulerCB);
}

void Interface::end() {

    char buf[1] = {SHUTDOWN_NOTIFIER};

    write(notifierPipe[1], buf, 1);

    threadRcv.join();

}

bool Interface::initThread() {

    if (pipe(notifierPipe) < 0) {
        LOGS_E(getHost(), "Notifier Pipe Init failed with err : %d!!!", errno);
        return false;
    }

    LOGS_T(getHost(), "Init Notifier PIPE OK!!!");

    threadRcv = std::thread([](Interface *interface) {

        interface->runReceiver();

    }, this);

    return true;
}

Interface::~Interface() {

    LOGP_T("Deallocating Interface");

    delete schedulerCB;

    delete scheduler;

    close(notifierPipe[1]);
    close(notifierPipe[0]);
}

bool Interface::push(MSG_DIR type, const TypeCommUnit& target, TypeMessage msg) {

    if (target->getAddress().getInterface() == getType()) {

        auto msgItem = std::make_shared<MessageItem>(type, target, std::move(msg));

        scheduler->push(std::move(msgItem));

        return true;
    }

    LOGS_E(getHost(), "Interface is not suitable for target : %d", target->getAddress().get().base);

    return false;
}

Address &Interface::getAddress() {

    return address;
}

Address &Interface::getMulticastAddress() {

    return multicastAddress;
}

void Interface::setAddress(Address &_address) {

    address = _address;
}

void Interface::setMulticastAddress(Address &_multicastAddress) {

    multicastAddress = _multicastAddress;
}

const TypeHostUnit& Interface::getHost() const {

    return host;
}

const TypeDevice& Interface::getDevice() const {

    return device;
}
