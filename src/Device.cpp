//
// Created by Haluk AKGUNDUZ on 12.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "Device.h"

Device::Device(std::string _name, COMM_INTERFACE _type, uint32_t _base, uint8_t _mask, bool _loopback)
    : name(std::move(_name)), type(_type), base(_base), mask(_mask), loopback(_loopback) {
}

Device::~Device() {

    LOGP_T("Deallocating Device");
}

const std::string& Device::getName() const {

    return name;
}

COMM_INTERFACE Device::getType() const {

    return type;
}

uint32_t Device::getBase() const {

    return base;
}

uint8_t Device::getMask() const {

    return mask;
}

bool Device::isLoopback() const {

    return loopback;
}
