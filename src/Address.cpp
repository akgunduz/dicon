//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"
#include "Net.h"
#include "UnixSocket.h"

long Address::createAddress(INTERFACES interface, long base, int port, int helper) {

    return ((long)(interface & INTERFACE_MASK) << 56) |
            ((long)(helper & HELPER_MASK) << 48) |
            ((long)(port & PORT_MASK) << 32) |
            (base & ADDRESS_MASK);
}

INTERFACES Address::getInterface(long address) {

    return (INTERFACES)((address >> 56) & INTERFACE_MASK);
}

long Address::getBase(long address) {

    return address & ADDRESS_MASK;
}

int Address::getPort(long address) {

    return (int)((address >> 32) & PORT_MASK);
}

int Address::getHelper(long address) {

    return (int)((address >> 48) & HELPER_MASK);

}

std::string Address::getString(long address) {

    INTERFACES _interface = getInterface(address);

    switch (_interface) {

        case INTERFACE_NET:
            return Net::getAddressString(address);

        case INTERFACE_UNIXSOCKET:
            return UnixSocket::getAddressString(address);

        default:
            return "Invalid Address";
    }
}
