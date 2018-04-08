//
// Created by Haluk AKGUNDUZ on 02/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_INTERFACETYPES_H
#define BANKOR_INTERFACETYPES_H

#include "Common.h"

enum INTERFACE {
    INTERFACE_NET,
    INTERFACE_UNIXSOCKET,
    INTERFACE_MAX
};

class Device;

typedef std::vector<long> (*fGetAddressList)(Device*);

class InterfaceTypes {
public:
    static fGetAddressList getAddressList(INTERFACE);
    static std::string getAddressString(long);
    static const char* getName(INTERFACE);
};


#endif //BANKOR_INTERFACETYPES_H
