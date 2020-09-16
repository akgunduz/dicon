//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "CommInterfaceFactory.h"
#include "CommTCP.h"
#include "CommUnixSocket.h"

TypeInterface CommInterfaceFactory::createInterface(const TypeHostUnit& host, const TypeDevice& device,
                                                    const InterfaceSchedulerCB *receiverCB) {

    TypeInterface commInterface = nullptr;

    switch (device->getType()) {

        case COMMINTERFACE_TCPIP:
            commInterface = new CommTCP(host, device, receiverCB);
            break;

        case COMMINTERFACE_UNIXSOCKET:
            commInterface = new CommUnixSocket(host, device, receiverCB);
            break;

        default:
            return nullptr;
    }

   // LOGP_E("Interface[%s] Use Count : %d", ComponentType::getName(host->getType()), commInterface.use_count());

    commInterface->initThread();

   // LOGP_E("Interface[%s] Use Count : %d", ComponentType::getName(host->getType()), commInterface.use_count());

    return commInterface;
}
