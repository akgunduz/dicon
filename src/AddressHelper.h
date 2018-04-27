//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_ADDRESS_H
#define BANKOR_ADDRESS_H

#include "InterfaceTypes.h"

#define INTERFACE_MASK 0x01
#define ADDRESS_MASK 0xFFFFFFFF
#define PORT_MASK 0xFFFF
#define ID_MASK 0x1FF
#define HELPER_MASK 0x1F

class AddressHelper {

private:
    AddressHelper(){};
    
public:

    static INTERFACE getInterface(long);
    static long getBase(long);
    static int getPort(long);
    static int getHelper(long);
    static int address2prefix(long);

    static long createAddress(INTERFACE, long, int, int);
};

#endif //BANKOR_ADDRESS_H
