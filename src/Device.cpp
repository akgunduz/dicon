//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "Device.h"

Device::Device(const char *_name, INTERFACES type, long address, long helper, bool loopback) {

    strcpy(this->name, _name);
    this->type = type;
    this->address = address;
    this->helper = helper;
    this->loopback = loopback;
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

void Device::setAddressList(std::vector<long> list) {
    addressList = list;
}

std::vector<long> Device::getAddressList() {
    return addressList;
}


