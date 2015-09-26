//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"
#include "NetAddress.h"
#include "UnixSocketAddress.h"
#include "PipeAddress.h"

Address::Address() {

}

Address::Address(long address) {
    this->address = address;
}

long Address::getAddress() {
    return address;
}


void Address::set(long address) {
    this->address = address;
}

Address *Address::newInstance(INTERFACES interface, long refAddress) {

    Address *newAddress = nullptr;

    switch(interface) {

        case INTERFACE_NET:
            newAddress = new NetAddress(refAddress);
            break;

        case INTERFACE_PIPE:
            newAddress = new PipeAddress(refAddress);
            break;

        case INTERFACE_UNIXSOCKET:
            newAddress = new UnixSocketAddress(refAddress);
            break;

        default:
            break;

    }

    return newAddress;
}

Address *Address::newInstance(long refAddress) {

    INTERFACES interface = INTERFACE_PIPE;

    if (refAddress > 0xFFFFFF) {
        interface = INTERFACE_NET;

    } else if (refAddress > 1000) {
        interface = INTERFACE_UNIXSOCKET;
    }

    return newInstance(interface, refAddress);
}

