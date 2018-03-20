//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "Device.h"

Device::Device(const char *name, INTERFACES type) {

    strcpy(this->name, name);
    this->type = type;
    this->address = 0;
    this->helper = 0;
    this->loopback = false;
}

Device::Device(const char *_name, long _ipAddress, long _helper, bool _loopback) {

    strcpy(this->name, _name);
    this->type = INTERFACE_NET;
    this->address = _ipAddress;
    this->helper = _helper;
    this->loopback = _loopback;
}

const char* Device::getName() {
    return name;
}

INTERFACES Device::getType() {
    return type;
}

long Device::getAddress() {
    return address;
}

long Device::getHelper() {
    return helper;
}

bool Device::isLoopback() {
    return loopback;
}

void Device::setPort(int port) {
    this->port = port;
}
