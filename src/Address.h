//
// Created by Haluk AKGUNDUZ on 27.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_ADDRESS_H
#define BANKOR_ADDRESS_H


#include "InterfaceTypes.h"

#define INTERFACE_MASK 0x01
#define ADDRESS_MASK 0xFFFFFFFF
#define PORT_MASK 0xFFFF
#define ID_MASK 0x1FF
#define HELPER_MASK 0x1F

class Address {

    INTERFACE interface;
    int id;
    int port;
    int helper;
    long base;

public:

    Address(INTERFACE, long, int, int, int = 0);
    Address(long);
    INTERFACE getInterface();
    int getID();
    long getBase();
    int getPort();
    int getHelper();
    int address2prefix();
    long serialize();
    void deserialize(long);
};


#endif //BANKOR_ADDRESS_H
