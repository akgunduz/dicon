//
// Created by akgunduz on 02.04.2018.
//

#include "InterfaceTypes.h"
#include "Net.h"
#include "UnixSocket.h"

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

    INTERFACE type = Address::getInterface(address);

    switch (type) {

        case INTERFACE_NET:
            return Net::getAddressString(address);

        case INTERFACE_UNIXSOCKET:
            return UnixSocket::getAddressString(address);

        default:
            return "Invalid Address";
    }
}
