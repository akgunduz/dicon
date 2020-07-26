//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef DICON_ADDRESS_H
#define DICON_ADDRESS_H

#include "InterfaceTypes.h"

#define INTERFACE_MASK 0x8000000000000000 //1bit
#define MULTICAST_MASK 0x4000000000000000 //1bit
#define SOCKET_MASK 0x3FFF000000000000 //14bit
#define PORT_MASK 0xFFFF00000000 //16bit
#define BASE_MASK 0xFFFFFFFF //32bit

#define INTERFACE_POS 63
#define MULTICAST_POS 62
#define SOCKET_POS 48
#define PORT_POS 32
#define BASE_POS 0

#define GET(a, b, c) (((a) & (c)) >> (b))
#define SET(a, b, c) ((((long)(a)) << (b)) & (c))
#define CLEAR_AND_SET(a, b, c, d) (((a) & ~(d)) | SET ((b), (c), (d)))

class Address {

private:
    Address()= default;
    
public:

    static INTERFACE getInterface(long&);
    static long getBase(long&);
    static int getPort(long&);
    static int getSocket(long&);
    static void setSocket(long&, int);
    static bool isMulticast(long&);
    static void setMulticast(long&, bool);

    static long create(INTERFACE, long, int, int = 0, bool = 0);
};

#endif //DICON_ADDRESS_H
