//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_UNIXSOCKETADDRESS_H
#define BANKOR_UNIXSOCKETADDRESS_H

#include "Common.h"
#include "Address.h"

class UnixSocketAddress  : public Address {

    sockaddr_un unix_addr;

public:

    UnixSocketAddress(long address);
    ~UnixSocketAddress(){}

    void init();

    INTERFACES getInterface();
    std::string getString();

    void set(long address);

    sockaddr_un getUnixAddress();

};


#endif //BANKOR_ADDRESS_H
