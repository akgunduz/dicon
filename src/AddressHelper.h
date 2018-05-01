//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_ADDRESS_H
#define BANKOR_ADDRESS_H

#include "InterfaceTypes.h"

#define INTERFACE_MASK 0x4000000000000000
#define ID_MASK 0x3FC0000000000000
#define HELPER_MASK 0x3F000000000000
#define PORT_MASK 0xFFFF00000000
#define BASE_MASK 0xFFFFFFFF

#define INTERFACE_POS 62
#define ID_POS 54
#define HELPER_POS 48
#define PORT_POS 32
#define BASE_POS 0

#define GET(a, b, c) (((a) & (c)) >> (b))
#define SET(a, b, c) ((((long)(a)) << (b)) & (c))
#define CLEAR_AND_SET(a, b, c, d) (((a) & ~(d)) | SET ((b), (c), (d)))

class AddressHelper {

private:
    AddressHelper(){};
    
public:

    static INTERFACE getInterface(long);
    static long getBase(long);
    static int getPort(long);
    static int getHelper(long);
//    static int getID(long);
//    static long setID(long, int);
    static int address2prefix(long);

    static long createAddress(INTERFACE, long, int, int, int = 0);
};

#endif //BANKOR_ADDRESS_H
