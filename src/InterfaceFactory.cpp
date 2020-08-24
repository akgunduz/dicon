//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "InterfaceFactory.h"
#include "Net.h"
#include "UnixSocket.h"

TypeInterface InterfaceFactory::createInterface(const TypeHostUnit& host, const TypeDevice& device,
                                                const InterfaceSchedulerCB *schedulerCB) {

    TypeInterface interface = nullptr;

    try {

        switch (device->getType()) {

            case INTERFACE_NET:
                interface = std::make_shared<Net>(host, device, schedulerCB);
                break;

            case INTERFACE_UNIXSOCKET:
                interface = std::make_shared<UnixSocket>(host, device, schedulerCB);
                break;

            default:
                break;
        }
    } catch (std::exception &e) {

        LOGS_E(host, "%s", e.what());
    }

    return interface;
}