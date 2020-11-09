//
// Created by Haluk AKGUNDUZ on 04.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//


#include "UtilNet.h"
#include "Util.h"

std::string UtilNet::getIPPortString(BaseAddress& address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%u", getIPString(address).c_str(), address.port);
    return std::string(sAddress);
}

std::string UtilNet::getIPString(uint32_t address) {

    struct in_addr addr{};
    addr.s_addr = htonl(address);
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        return "";
    }

    return std::string(cIP);
}

std::string UtilNet::getIPString(BaseAddress& address) {

    return getIPString(address.base);
}

std::string UtilNet::getIPString(const sockaddr_in *address) {

    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &(address->sin_addr), cIP, INET_ADDRSTRLEN);
    if (!dst) {
        return "";
    }

    return std::string(cIP);
}

long UtilNet::parseIPAddress(const std::string& address) {

    struct in_addr addr{};

    int res = inet_pton(AF_INET, address.c_str(), &addr);
    if (res <= 0) {
        return 0;
    }

    return ntohl(addr.s_addr);
}

BaseAddress UtilNet::parseIPPortString(const std::string& address) {

    size_t pos = address.find(':');

    std::string sIP = address.substr(0, pos);

    std::string sPort = address.substr(pos + 1, std::string::npos);

    long ip = parseIPAddress(sIP);
    int port = atoi(sPort.c_str());

    return BaseAddress(ip, port);
}


sockaddr_in UtilNet::getInetAddressByAddress(TypeAddress& address) {

    sockaddr_in inet_addr{};

    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(address->get().port);
    inet_addr.sin_addr.s_addr = htonl(address->get().base);
    return inet_addr;
}

sockaddr_in UtilNet::getInetAddressByPort(int port) {

    sockaddr_in inet_addr{};

    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(port);
    inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return inet_addr;
}

ip_mreq UtilNet::getInetMulticastAddress(TypeAddress& address, TypeAddress& multicastAddress) {

    ip_mreq imReq{};

    imReq.imr_multiaddr.s_addr = htonl(multicastAddress->get().base);
    imReq.imr_interface.s_addr = htonl(address->get().base);
    return imReq;
}

sockaddr_un UtilNet::getUnixAddress(TypeAddress& address) {

    sockaddr_un unix_addr{};
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s/%s%u_%u%s",
            Util::tmpPath.string().c_str(),
            UNIXSOCKET_FILE_PREFIX,
            address->get().base,
            address->get().port,
            UNIXSOCKET_FILE_SUFFIX);
    return unix_addr;
}

std::string UtilNet::getUnixString(BaseAddress& address) {

    char sAddress[50];
    sprintf(sAddress, "%u", address.base);
    return std::string(sAddress);
}

void UtilNet::cleanUnixPath() {

    for (const auto& entry : std::filesystem::directory_iterator(Util::tmpPath)) {

        if (!entry.is_socket() ||
            entry.path().filename().string().find(UNIXSOCKET_FILE_PREFIX) == std::string::npos) {
            continue;
        }

        std::filesystem::remove(entry.path());
    }
}