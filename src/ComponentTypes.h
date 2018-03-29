//
// Created by Haluk AKGUNDUZ on 29/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_COMPONENTTYPES_H
#define BANKOR_COMPONENTTYPES_H

#include "Common.h"

enum COMPONENT {
    COMP_DISTRIBUTOR,
    COMP_COLLECTOR,
    COMP_NODE,
    COMP_MAX
};

class ComponentTypes {

public:
    static const char* getName(COMPONENT);
    static UI_UPDATE getAssignedUILog(COMPONENT);

};


#endif //BANKOR_COMPONENTTYPES_H
