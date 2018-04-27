//
// Created by Haluk AKGUNDUZ on 02/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "InterfaceTypes.h"
#include "Net.h"
#include "UnixSocket.h"

const char* sInterfaceNames[INTERFACE_MAX] = {
        "TCP",
        "UnixSocket",
};

fGetAddressList InterfaceTypes::getAddressList(INTERFACE type) {

    switch (type) {

        case INTERFACE_NET:
            return Net::getAddressList;

        case INTERFACE_UNIXSOCKET:
            return UnixSocket::getAddressList;

        default:
            LOG_E("Undefined Interface Type : %d", type);
            break;
    }

    return NULL;
}

std::string InterfaceTypes::getAddressString(long address) {

    INTERFACE type = AddressHelper::getInterface(address);

    switch (type) {

        case INTERFACE_NET:
            return Net::getAddressString(address);

        case INTERFACE_UNIXSOCKET:
            return UnixSocket::getAddressString(address);

        default:
            return "Invalid Address";
    }
}

const char* InterfaceTypes::getName(INTERFACE type) {

    return sInterfaceNames[type];
}
