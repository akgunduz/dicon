//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "Device.h"

Device::Device(const char *_name, INTERFACE type, long base, int helper, bool loopback) {

    strcpy(this->name, _name);
    this->type = type;
    this->base = base;
    this->helper = helper;
    this->loopback = loopback;
}

const char* Device::getName() {
    return name;
}

INTERFACE Device::getType() {
    return type;
}

long Device::getBase() {
    return base;
}

int Device::getHelper() {
    return helper;
}

bool Device::isLoopback() {
    return loopback;
}

std::vector<long> Device::getAddressList() {
    return InterfaceTypes::getAddressList(getType())(this);
}


