//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"
#include "NetAddress.h"

INTERFACES Address::getInterface(long address) {

    if (address > 0xFFFFFF) {
        return INTERFACE_NET;

    } else if (address > 1000) {
        return INTERFACE_UNIXSOCKET;
    }

    return INTERFACE_MAX;
}

inline std::string Address::getStdString(long address) {

    char sAddress[50];
    sprintf(sAddress, "%ld", address);
    return std::string(sAddress);

}

std::string Address::getString(long address) {

    INTERFACES _interface = getInterface(address);

    switch (_interface) {

        case INTERFACE_NET:
            return NetAddress::getString(address);

        default:
            return getStdString(address);
    }
}