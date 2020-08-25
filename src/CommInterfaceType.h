//
// Created by Haluk AKGUNDUZ on 02/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMINTERFACETYPE_H
#define DICON_COMMINTERFACETYPE_H

#include "Common.h"

enum COMM_INTERFACE {

    COMMINTERFACE_TCPIP,
    COMMINTERFACE_UNIXSOCKET,
    COMMINTERFACE_MAX
};

class CommInterfaceType {

    static constexpr const char* sInterfaceNames[COMMINTERFACE_MAX] = {
            "TCP",
            "UnixSocket",
    };

public:

    static const char* getName(COMM_INTERFACE _type) {

        return sInterfaceNames[_type];
    }
};

#endif //DICON_COMMINTERFACETYPE_H
