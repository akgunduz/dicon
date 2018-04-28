//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "AddressHelper.h"

long AddressHelper::createAddress(INTERFACE interface, long base, int port, int helper, int id) {

    return SET(interface, INTERFACE_POS, INTERFACE_MASK) |
           SET(id, ID_POS, ID_MASK) |
           SET(helper, HELPER_POS, HELPER_MASK) |
           SET(port, PORT_POS, PORT_MASK) |
           SET(base, BASE_POS, BASE_MASK) ;
}

INTERFACE AddressHelper::getInterface(long address) {

    return (INTERFACE) GET(address, INTERFACE_POS, INTERFACE_MASK);
}

long AddressHelper::getBase(long address) {

    return GET(address, BASE_POS, BASE_MASK);
}

int AddressHelper::getPort(long address) {

    return (int) GET(address, PORT_POS, PORT_MASK);
}

int AddressHelper::getHelper(long address) {

    return (int) GET(address, HELPER_POS, HELPER_MASK);
}

int AddressHelper::getID(long address) {

    return (int) GET(address, ID_POS, ID_MASK);
}

long AddressHelper::setID(long address, int id) {

    return CLEAR_AND_SET(address, id, ID_POS, ID_MASK);
}

int AddressHelper::address2prefix(long address) {

    int i = 0;
    uint32_t ip = (uint32_t) AddressHelper::getBase(address);
    while(ip > 0) {
        ip = ip >> 1;
        i++;
    }

    return i;
}
