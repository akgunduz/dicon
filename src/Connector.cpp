//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "UnixSocket.h"

Interface* Connector::createInterface(Device *device,
                                      const InterfaceSchedulerCB *scb,
                                      const InterfaceHostCB *hcb) {

    Interface *interface = nullptr;

    switch(device->getType()) {

        case INTERFACE_NET:
            interface = new Net(device, scb, hcb);
            break;

        case INTERFACE_UNIXSOCKET:
            interface = new UnixSocket(device, scb, hcb);
            break;

        default:
            break;
    }

    return interface;
}
