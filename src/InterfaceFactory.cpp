//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "InterfaceFactory.h"
#include "Net.h"
#include "UnixSocket.h"

TypeInterface InterfaceFactory::createInterface(TypeDevice& device,
                                                const InterfaceSchedulerCB *schedulerCB,
                                                const InterfaceHostCB *hostCB) {

    TypeInterface interface = nullptr;

    try {

        switch (device->getType()) {

            case INTERFACE_NET:
                interface = std::make_shared<Net>(device, schedulerCB, hostCB);
                break;

            case INTERFACE_UNIXSOCKET:
                interface = std::make_shared<UnixSocket>(device, schedulerCB, hostCB);
                break;

            default:
                break;
        }
    } catch (std::exception &e) {

        LOGS_E(hostCB->hostCB(hostCB->arg), "%s", e.what());
    }

    return interface;
}
