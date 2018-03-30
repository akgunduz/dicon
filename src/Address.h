//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_ADDRESS_H
#define BANKOR_ADDRESS_H

#include "Common.h"

#define INTERFACE_MASK 0x01
#define ADDRESS_MASK 0xFFFFFFFF
#define PORT_MASK 0xFFFF
#define HELPER_MASK 0xFF

class Address {

private:
    Address(){};


public:

    static INTERFACES getInterface(long address);
    static long getBase(long);
    static int getPort(long);
    static int getHelper(long);

    static std::string getString(long address);
    static long createAddress(INTERFACES, long base, int port, int helper);
};

#endif //BANKOR_ADDRESS_H
