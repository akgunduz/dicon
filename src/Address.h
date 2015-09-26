//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_ADDRESS_H
#define BANKOR_ADDRESS_H


#include "Common.h"

class Address {

protected:

    long address;

public:

    Address();
    virtual ~Address(){};

    Address(long address);

    virtual INTERFACES getInterface() = 0;
    virtual std::string getString() = 0;

    long getAddress();
    void set(long address);

    static Address* newInstance(INTERFACES, long refAddress = 0);
    static Address* newInstance(long);
};


#endif //BANKOR_ADDRESS_H
