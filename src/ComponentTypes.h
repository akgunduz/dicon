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

    static char rootPath[COMP_MAX][PATH_MAX];

public:
    static const char* getName(COMPONENT);

    static COMPONENT getNextType(COMPONENT);
    static COMPONENT getPrevType(COMPONENT);

    static const char* getRootPath(COMPONENT);
    static void setRootPath(COMPONENT, const char*);

};


#endif //BANKOR_COMPONENTTYPES_H
