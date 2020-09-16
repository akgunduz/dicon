//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterfaceFactory.h"
#include "CommTCP.h"
#include "CommUnixSocket.h"

TypeInterface CommInterfaceFactory::createInterface(TypeHostUnit host, const TypeDevice& device,
                                                    const CommInterfaceCB *receiverCB) {

    TypeInterface commInterface;

    switch (device->getType()) {

        case COMMINTERFACE_TCPIP:
            commInterface = new CommTCP(std::move(host), device, receiverCB);
            break;

        case COMMINTERFACE_UNIXSOCKET:
            commInterface = new CommUnixSocket(std::move(host), device, receiverCB);
            break;

        default:
            return nullptr;
    }

    commInterface->initThread();

    return commInterface;
}
