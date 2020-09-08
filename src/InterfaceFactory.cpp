//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "InterfaceFactory.h"
#include "CommTCP.h"
#include "CommUnixSocket.h"

TypeInterface InterfaceFactory::createInterface(const TypeHostUnit& host, const TypeDevice& device,
                                                const InterfaceSchedulerCB *receiverCB) {

    TypeInterface commInterface = nullptr;

    try {

        switch (device->getType()) {

            case COMMINTERFACE_TCPIP:
                commInterface = std::make_shared<CommTCP>(host, device, receiverCB);
                break;

            case COMMINTERFACE_UNIXSOCKET:
                commInterface = std::make_shared<CommUnixSocket>(host, device, receiverCB);
                break;

            default:
                break;
        }
    } catch (std::exception &e) {

        LOGS_E(host, "%s", e.what());

        return nullptr;
    }

    return commInterface;
}
