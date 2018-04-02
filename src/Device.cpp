//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "Device.h"

Device::Device() {

    strcpy(this->name, "null");
    this->type = INTERFACE_MAX;
}

Device::Device(const char *_name, INTERFACES type, long base, int helper, bool loopback) {

    strcpy(this->name, _name);
    this->type = type;
    this->base = base;
    this->helper = helper;
    this->loopback = loopback;
    this->address = 0;
    this->multicastAddress = 0;
}

const char* Device::getName() {
    return name;
}

INTERFACES Device::getType() {
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

void Device::setAddressList(fGetAddressList getAddressList) {
    this->getAddressListFunc = getAddressList;
}

std::vector<long> Device::getAddressList() {
    return getAddressListFunc(this);
}

long Device::getAddress() {
    return address;
}

void Device::setAddress(long address) {
    this->address = address;
}

void Device::setMulticastAddress(long multicastAddress) {
    this->multicastAddress = multicastAddress;
}

long Device::getMulticastAddress() {
    return multicastAddress;
}

void Device::set(Device *device) {

    strcpy(this->name, device->getName());
    this->type = device->getType();
    this->base = device->getBase();
    this->helper = device->getHelper();
    this->loopback = device->isLoopback();
    this->address = device->getAddress();
    this->multicastAddress = device->getMulticastAddress();
    this->getAddressListFunc = device->getAddressListFunc;
}


