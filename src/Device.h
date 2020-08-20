//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef DICON_DEVICE_H
#define DICON_DEVICE_H

#include "InterfaceTypes.h"

class Device {

    char name[NAME_MAX]{};

    INTERFACE type;

    long base;
    int mask;
    bool loopback;

public:

    Device(const char*, INTERFACE, uint32_t , uint8_t = 0, bool = false);
    ~Device();

    const char* getName() const;
    INTERFACE getType() const;
    uint32_t getBase() const;
    uint8_t getMask() const;
    bool isLoopback() const;

};

#endif //DICON_DEVICE_H
