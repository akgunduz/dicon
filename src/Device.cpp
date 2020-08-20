//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "Device.h"
#include "Log.h"

Device::Device(const char *_name, INTERFACE _type, uint32_t _base, uint8_t _mask, bool _loopback)
    : type(_type), base(_base), mask(_mask), loopback(_loopback) {

    strcpy(this->name, _name);
}

Device::~Device() {

    PRINT("Deallocating Device: %s", name);
}

const char* Device::getName() const {

    return name;
}

INTERFACE Device::getType() const {

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
