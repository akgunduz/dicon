//
// Created by Haluk AKGUNDUZ on 02/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "InterfaceType.h"
#include "Net.h"
#include "UnixSocket.h"

const char* sInterfaceNames[INTERFACE_MAX] = {
        "TCP",
        "UnixSocket",
};

const char* InterfaceType::getName(INTERFACE type) {

    return sInterfaceNames[type];
}
