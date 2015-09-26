//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_PIPEADDRESS_H
#define BANKOR_PIPEADDRESS_H


#include "Common.h"
#include "Address.h"

class PipeAddress : public Address {

public:

    PipeAddress(long address);
    ~PipeAddress(){}

    INTERFACES getInterface();
    std::string getString();

    void set(long address);

};


#endif //BANKOR_ADDRESS_H
