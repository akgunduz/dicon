//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef BANKOR_DEVICE_H
#define BANKOR_DEVICE_H

#include "InterfaceTypes.h"

class Device {

    char name[50];
    INTERFACE type;

    long base;
    int helper;
    bool loopback;

public:

    Device(const char*, INTERFACE, long, int, bool = false);

    const char* getName();
    INTERFACE getType();
    long getBase();
    int getHelper();
    bool isLoopback();

    std::vector<long> getAddressList();
};


#endif //BANKOR_DEVICE_H
