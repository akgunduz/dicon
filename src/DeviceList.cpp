//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include <random>
#include "DeviceList.h"
#include "Address.h"

DeviceList* DeviceList::instance = nullptr;

int createMask(uint32_t baseAddress) {

    int i = 0;
    auto ip = baseAddress;
    while(ip > 0) {
        ip = ip >> 1;
        i++;
    }

    return i;
}

DeviceList::DeviceList() {

    Device *device = nullptr;
    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return;
    }

    for (loop = ifAddrStruct; loop != nullptr; loop = loop->ifa_next) {

        if (loop->ifa_addr->sa_family == AF_INET) { // check if it is IP

            if (!((loop->ifa_flags & IFF_UP) && (loop->ifa_flags & IFF_RUNNING))) {
                continue;
            }

            device = new Device(loop->ifa_name, INTERFACE_NET,
                                ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                createMask(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)),
                                               (loop->ifa_flags & IFF_LOOPBACK) > 0);

            add(device);
        }
    };

    freeifaddrs(ifAddrStruct);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(1, time(nullptr));

    device = new Device("us", INTERFACE_UNIXSOCKET, getpid(), distribution(generator));

    add(device);
}

DeviceList *DeviceList::getInstance() {

    if (instance == nullptr) {
        instance = new DeviceList();
    }
    return instance;
}

bool DeviceList::add(Device *device) {

    list.push_back(device);
    return false;
}

Device* DeviceList::get(int index) {

    return list[index];
}

long DeviceList::getCount() {

    return list.size();
}
