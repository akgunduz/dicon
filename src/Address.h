//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_ADDRESS_H
#define BANKOR_ADDRESS_H


#include "Common.h"

class Address {

    Address(){};

    static std::string getStdString(long address);

public:

    static INTERFACES getInterface(long address);
    static std::string getString(long address);

};


#endif //BANKOR_ADDRESS_H
