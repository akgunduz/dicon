//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "DeviceList.h"
#include "Address.h"
#include "Log.h"
#include "Util.h"

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

#ifndef WIN32

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

            add(std::make_shared<Device>(loop->ifa_name, COMMINTERFACE_TCPIP,
                                         ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                         createMask(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)),
                                         (loop->ifa_flags & IFF_LOOPBACK) > 0));
        }
    };

    freeifaddrs(ifAddrStruct);

#else

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        LOGP_E("WSAStartup failed: %d", iResult);
        return;
    }

    unsigned long outBufLen = 15000;

    auto pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);

    auto status = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &outBufLen);

    if (status == ERROR_BUFFER_OVERFLOW) {
        free(pAddresses);
        return;
    }

    for (auto pCurrAddress = pAddresses; pCurrAddress; pCurrAddress = pCurrAddress->Next) {

        if (pCurrAddress->OperStatus != IfOperStatusUp) {
            continue;
        }

        auto *si = (sockaddr_in *)(pCurrAddress->FirstUnicastAddress->Address.lpSockaddr);

        add(std::make_shared<Device>(Util::to_narrow(pCurrAddress->FriendlyName),
                                     COMMINTERFACE_TCPIP,
                                     ntohl(si->sin_addr.s_addr),
                                     pCurrAddress->FirstUnicastAddress->OnLinkPrefixLength,
                                     pCurrAddress->IfType == IF_TYPE_SOFTWARE_LOOPBACK));

    }

    free(pAddresses);

#endif

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(1, time(nullptr));

    add(std::make_shared<Device>("us", COMMINTERFACE_UNIXSOCKET, distribution(generator)));
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

#ifdef WIN32
    WSACleanup();
#endif
}
