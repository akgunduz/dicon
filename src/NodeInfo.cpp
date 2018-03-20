//
// Created by Haluk AKGUNDUZ on 19/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "NodeInfo.h"

NodeInfo::NodeInfo() {

    address = 0;
    arch = ARCH_MAX;
}

NodeInfo::NodeInfo(const NodeInfo &rep) {

    this->address = rep.address;
    this->arch = rep.arch;
}


NodeInfo::NodeInfo(long address, ARCH arch) {

    this->address = address;
    this->arch = arch;
}

long NodeInfo::getAddress() {

    return address;
}


bool NodeInfo::setAddress(long address) {

    this->address = address;
    return true;
}

ARCH NodeInfo::getArch() {

    return arch;
}

bool NodeInfo::setArch(ARCH arch) {

    this->arch = arch;
    return true;
}

bool NodeInfo::setNode(long address, ARCH arch) {

    this->address = address;
    this->arch = arch;
    return true;
}

bool NodeInfo::isValid() {

    return address > 0 && arch < ARCH_MAX;
}

void NodeInfo::reset() {

    address = 0;
    arch = ARCH_MAX;
}
