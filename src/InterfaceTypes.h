//
// Created by Haluk AKGUNDUZ on 02/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_INTERFACETYPES_H
#define DICON_INTERFACETYPES_H

#include "Common.h"

enum INTERFACE {
    INTERFACE_NET,
    INTERFACE_UNIXSOCKET,
    INTERFACE_MAX
};

//class Device;

//typedef std::vector<long> (*fGetAddressList)(Device*);

class InterfaceTypes {
public:
//    static fGetAddressList getAddressList(INTERFACE);
    static const char* getName(INTERFACE);
};


#endif //DICON_INTERFACETYPES_H
