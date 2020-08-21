//
// Created by Haluk AKGUNDUZ on 02/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_INTERFACETYPE_H
#define DICON_INTERFACETYPE_H

#include "Common.h"

enum INTERFACE {
    INTERFACE_NET,
    INTERFACE_UNIXSOCKET,
    INTERFACE_MAX
};

class InterfaceType {
public:

    static const char* getName(INTERFACE);
};

#endif //DICON_INTERFACETYPE_H
