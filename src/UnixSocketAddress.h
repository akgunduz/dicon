//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_UNIXSOCKETADDRESS_H
#define BANKOR_UNIXSOCKETADDRESS_H

#include "Common.h"
#include "Address.h"
#include "Device.h"

class UnixSocketAddress  : public Address {

public:
    static sockaddr_un getUnixAddress(long);
};


#endif //BANKOR_ADDRESS_H
