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

Device::Device(const char *_name, long _ipAddress, long _helper) {

    strcpy(this->name, _name);
    this->type = INTERFACE_NET;
    this->address = _ipAddress;
    this->helper = _helper;
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

bool Device::createInterfaces() {

    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return false;
    }

    for (loop = ifAddrStruct; loop != NULL; loop = loop->ifa_next) {

        if (loop->ifa_addr->sa_family == AF_INET) { // check it is IP4

            if (strncmp(loop->ifa_name, "et", 2) == 0 ||
                strncmp(loop->ifa_name, "en", 2) == 0 ||
                strncmp(loop->ifa_name, "br", 2) == 0 ||
                strncmp(loop->ifa_name, "lo", 2) == 0 ||
                strncmp(loop->ifa_name, "wlan", 2) == 0) {

                deviceList.push_back(Device(loop->ifa_name,
                                               ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                               NetAddress::address2prefix(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr))));

            }
        }
    };

    freeifaddrs(ifAddrStruct);

    deviceList.push_back(Device("us", INTERFACE_UNIXSOCKET));

    return true;
}

Device* Device::getFreeDevice(CONNECTTYPE connectType) {

    switch(connectType) {

        case CONNECT_TCP:

            break;
    }

    return nullptr;
}

void Device::setPort(int port) {
    this->port = port;
}
