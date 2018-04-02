//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef BANKOR_CONNECTINTERFACE_H
#define BANKOR_CONNECTINTERFACE_H

#include "Common.h"

class Device;

typedef std::vector<long> (*fGetAddressList)(Device*);

class Device {

    char name[50];
    INTERFACES type;

    long base;
    int helper;
    bool loopback;

    long address;
    long multicastAddress;

    fGetAddressList getAddressListFunc;

public:

    Device(const char*, INTERFACES, long, int, bool = false);
    Device();

    const char* getName();
    INTERFACES getType();
    long getBase();
    long getAddress();
    long getMulticastAddress();
    int getHelper();
    bool isLoopback();

    void setAddressList(fGetAddressList);
    std::vector<long> getAddressList();

    void set(Device*);
    void setAddress(long);
    void setMulticastAddress(long);
};


#endif //BANKOR_CONNECTINTERFACE_H
