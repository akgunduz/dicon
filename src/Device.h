//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef BANKOR_CONNECTINTERFACE_H
#define BANKOR_CONNECTINTERFACE_H

#include "Common.h"

class Device {

    char name[50];
    INTERFACES type;

    long address;
    long helper;
    bool loopback;

    std::vector<long> addressList;

public:

    Device(const char*, INTERFACES, long, long, bool = false);

    const char* getName();
    INTERFACES getType();
    long getAddress();
    long getHelper();
    bool isLoopback();
    void setPort(int port);
    void setAddressList(std::vector<long>);
    std::vector<long> getAddressList();

};


#endif //BANKOR_CONNECTINTERFACE_H
