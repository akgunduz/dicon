//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"

long Address::createAddress(INTERFACE interface, long base, int port, int helper) {

    return ((long)(interface & INTERFACE_MASK) << 62) |
            ((long)(helper & ID_MASK) << 53) |
            ((long)(helper & HELPER_MASK) << 48) |
            ((long)(port & PORT_MASK) << 32) |
            (base & ADDRESS_MASK);
}

INTERFACE Address::getInterface(long address) {

    return (INTERFACE)((address >> 62) & INTERFACE_MASK);
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

int Address::getID(long address) {

    return (int)((address >> 53) & HELPER_MASK);
}

int Address::address2prefix(long address) {

    int i = 0;
    uint32_t ip = (uint32_t) Address::getBase(address);
    while(ip > 0) {
        ip = ip >> 1;
        i++;
    }

    return i;
}
