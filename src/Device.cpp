//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "Device.h"
#include "NetAddress.h"
#include "Net.h"
#include "UnixSocket.h"
#include "Pipe.h"

std::vector<Device> Device::deviceList;

Device::Device(const char *name, INTERFACES type) {

    strcpy(this->name, name);
    this->type = type;
}

Device::Device(const char *_name, long _ipAddress, long _helper, bool _multicastEnabled) {

    strcpy(this->name, _name);
    this->type = INTERFACE_NET;
    this->address = _ipAddress;
    this->helper = _helper;
    this->multicastEnabled = _multicastEnabled;
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

bool Device::isMulticastEnabled() {
    return multicastEnabled;
}



std::vector<Device> Device::getInterfaces() {

    if (deviceList.size() > 0) {
        return deviceList;
    }

    std::vector<Device> netInterfaces = Net::getInterfaces();
    deviceList.insert(deviceList.end(), netInterfaces.begin(), netInterfaces.end());

    std::vector<Device> unixSocketInterfaces = UnixSocket::getInterfaces();
    deviceList.insert(deviceList.end(), unixSocketInterfaces.begin(), unixSocketInterfaces.end());

    std::vector<Device> pipeInterfaces = Pipe::getInterfaces();
    deviceList.insert(deviceList.end(), pipeInterfaces.begin(), pipeInterfaces.end());

    return deviceList;
}


int Device::getCount() {

    return (int)deviceList.size();

}

Device *Device::getDevice(int index) {

    return &deviceList[index >= deviceList.size() ?
                       deviceList.size() - 1 : index];

}

std::string Device::getName(int index) {

    Device *ci = &deviceList[index >= deviceList.size() ?
                             deviceList.size() - 1 : index];

    return ci->name;
}

INTERFACES Device::getType(int index) {

    Device *ci = &deviceList[index >= deviceList.size() ? deviceList.size() - 1 : index];

    return ci->type;

}

long Device::getAddress(int index) {

    Device *ci = &deviceList[index >= deviceList.size() ?
                             deviceList.size() - 1 : index];

    return ci->address;
}

long Device::getHelper(int index) {

    Device *ci = &deviceList[index >= deviceList.size() ?
                             deviceList.size() - 1 : index];

    return ci->helper;
}

bool Device::isMulticastEnabled(int index) {

    Device *ci = &deviceList[index >= deviceList.size() ?
                             deviceList.size() - 1 : index];
    return ci->multicastEnabled;
}
