//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "uv.h"
#include "DeviceList.h"
#include "Address.h"
#include "Log.h"

int createMask(uint32_t baseAddress) {

    int i = 0;
    auto ip = baseAddress;
    while(ip > 0) {
        ip = ip >> 1u;
        i++;
    }

    return i;
}

DeviceList::DeviceList() {

    int count;
    uv_interface_address_t *info;

    uv_interface_addresses(&info, &count);

    for (int i = 0; i < count; i++) {

        uv_interface_address_t* iFace = &info[i];

        if (iFace->address.address4.sin_family != AF_INET) {
            continue;
        }

        add(std::make_shared<Device>(iFace->name, COMMINTERFACE_TCPIP,
                                     ntohl(iFace->address.address4.sin_addr.s_addr),
                                     createMask(ntohl(iFace->netmask.netmask4.sin_addr.s_addr)),
                                     iFace->is_internal));
    }

    uv_free_interface_addresses(info, count);

    add(std::make_shared<Device>("us", COMMINTERFACE_UNIXSOCKET, FAKE_US_ADDRESS, FAKE_US_MASK, true));
}

DeviceList *DeviceList::getInstance() {

    if (instance == nullptr) {

        instance = new DeviceList();
    }

    return instance;
}

bool DeviceList::add(TypeDevice device) {

    list.emplace_back(std::move(device));

    return true;
}

TypeDevice& DeviceList::get(int index) {

    return list[index];
}

size_t DeviceList::getCount() {

    return list.size();
}

DeviceList::~DeviceList() {

    LOGP_T("Deallocating DeviceList");
}
