//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "AddressHelper.h"

long AddressHelper::createAddress(INTERFACE interface, long base, int port, int helper) {

    return ((long)(interface & INTERFACE_MASK) << 62) |
            ((long)(helper & HELPER_MASK) << 48) |
            ((long)(port & PORT_MASK) << 32) |
            (base & ADDRESS_MASK);
}

INTERFACE AddressHelper::getInterface(long address) {

    return (INTERFACE)((address >> 62) & INTERFACE_MASK);
}

long AddressHelper::getBase(long address) {

    return address & ADDRESS_MASK;
}

int AddressHelper::getPort(long address) {

    return (int)((address >> 32) & PORT_MASK);
}

int AddressHelper::getHelper(long address) {

    return (int)((address >> 48) & HELPER_MASK);
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
