//
// Created by Haluk AKGUNDUZ on 19/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_NODEINFO_H
#define BANKOR_NODEINFO_H


#include "ArchTypes.h"

class NodeInfo {

    long address;
    ARCH arch;

public:
    NodeInfo();
    NodeInfo(const NodeInfo &rep);
    NodeInfo(long, ARCH);
    const long getAddress() const;
    bool setAddress(long);
    const ARCH getArch();
    bool setArch(ARCH);
    bool setNode(long, ARCH);
    bool isValid();
    void reset();

};


#endif //BANKOR_NODEINFO_H