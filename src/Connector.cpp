//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "UnixSocket.h"

Interface* Connector::createInterface(const Unit component, Device *device, const InterfaceCallback *cb) {

    Interface *interface = NULL;

    switch(device->getType()) {

        case INTERFACE_NET:
            interface = new Net(component, device, cb);
            break;

        case INTERFACE_UNIXSOCKET:
            interface = new UnixSocket(component, device, cb);
            break;

        default:
            break;
    }

    return interface;
}
