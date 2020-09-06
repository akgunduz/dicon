//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "InterfaceFactory.h"
#include "CommTCP.h"
#include "UnixSocket.h"

TypeInterface InterfaceFactory::createInterface(const TypeHostUnit& host, const TypeDevice& device,
                                                const InterfaceSchedulerCB *schedulerCB) {

    TypeInterface commInterface = nullptr;

    try {

        switch (device->getType()) {

            case COMMINTERFACE_TCPIP:
                commInterface = std::make_shared<CommTCP>(host, device, schedulerCB);
                break;

            case COMMINTERFACE_UNIXSOCKET:
                commInterface = std::make_shared<UnixSocket>(host, device, schedulerCB);
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
