//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include "DeviceList.h"
#include "Address.h"
#include "Log.h"

DeviceList* DeviceList::instance = NULL;

DeviceList::DeviceList() {

    Device *device = nullptr;
    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return;
    }

    for (loop = ifAddrStruct; loop != NULL; loop = loop->ifa_next) {

        if (loop->ifa_addr->sa_family == AF_INET) { // check it is IP4

            if (!((loop->ifa_flags & IFF_UP) && (loop->ifa_flags & IFF_RUNNING))) {
                continue;
            }

            device = new Device(loop->ifa_name, INTERFACE_NET,
                                               ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                               Address::address2prefix(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)),
                                               (loop->ifa_flags & IFF_LOOPBACK) > 0);

            add(device);
        }
    };

    freeifaddrs(ifAddrStruct);

    srand(time(NULL));
    device = new Device("us", INTERFACE_UNIXSOCKET, getpid(), rand());

    add(device);
}

DeviceList *DeviceList::getInstance() {

    if (instance == NULL) {
        instance = new DeviceList();
    }
    return instance;
}

bool DeviceList::add(Device *device) {

    devices.push_back(device);
    return false;
}

Device* DeviceList::get(int index) {

    return devices[index];
}

Device *DeviceList::getActive(int index) {

    return devices[active[index]];
}

void DeviceList::setActive(int index0, int index1) {

    active[0] = index0;
    active[1] = index1;
}

bool DeviceList::isActiveDifferent() {

    return getActive(0) != getActive(1);
}

long DeviceList::getCount() {

    return devices.size();
}
