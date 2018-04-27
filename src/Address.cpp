//
// Created by Haluk AKGUNDUZ on 27.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "Address.h"

Address::Address(INTERFACE interface, long base, int port, int helper, int id) :
    interface(interface), base(base), port(port), helper(helper), id(id) {

}

Address::Address(long address) {

    deserialize(address);
}

INTERFACE Address::getInterface() {

    return interface;
}

long Address::getBase() {

    return base;
}

int Address::getPort() {

    return port;
}

int Address::getHelper() {

    return helper;
}

int Address::getID() {

    return id;
}

int Address::address2prefix() {

    int i = 0;
    uint32_t ip = (uint32_t) base;
    while(ip > 0) {
        ip = ip >> 1;
        i++;
    }

    return i;
}

long Address::serialize() {

    return ((long)(interface & INTERFACE_MASK) << 62) |
           ((long)(id & ID_MASK) << 53) |
           ((long)(helper & HELPER_MASK) << 48) |
           ((long)(port & PORT_MASK) << 32) |
           (base & ADDRESS_MASK);
}

void Address::deserialize(long address) {

    interface = (INTERFACE)((address >> 62) & INTERFACE_MASK);
    id = (int)((address >> 53) & ID_MASK);
    base = address & ADDRESS_MASK;
    port = (int)((address >> 32) & PORT_MASK);
    helper = (int)((address >> 48) & HELPER_MASK);
}
