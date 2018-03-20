//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef BANKOR_CONNECTINTERFACE_H
#define BANKOR_CONNECTINTERFACE_H

#include "Common.h"

class Device {
public:
    char name[50];
    INTERFACES type;

    long address;
    long helper;
    bool loopback;
    int port;

    Device(const char* _name, INTERFACES _type);
    Device(const char* _name, long _ipAddress, long _helper, bool _loopback);

    const char* getName();
    INTERFACES getType();
    long getAddress();
    long getHelper();
    bool isLoopback();
    void setPort(int port);
};


#endif //BANKOR_CONNECTINTERFACE_H
