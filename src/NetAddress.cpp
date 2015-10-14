//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "NetAddress.h"
#include "Log.h"

std::string NetAddress::getString(long address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%d", getIPstr(address).c_str(), getPort(address));
    return std::string(sAddress);
}

long NetAddress::parseIP(const std::string &ip) {

    struct in_addr addr;

    int res = inet_pton(AF_INET, ip.c_str(), &addr);
    if (!res) {
        LOG_E("Ip is invalid");
        return 0;
    }

    return addr.s_addr;
}

long NetAddress::parseAddress(long ip, int port, int netmask) {

    return ((long)netmask << 48) | ((long)port << 32) | ip;

}

long NetAddress::getIP(long address) {

    return address & IPADDRESS_MASK;
}

std::string NetAddress::getIPstr(long address) {

    struct in_addr addr;
    addr.s_addr = (uint32_t)getIP(address);
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        LOG_E("Ip is invalid");
        return "";
    }

    return std::string(cIP);
}

int NetAddress::getPort(long address) {

    return (int)((address >> 32) & 0xFFFF);

}

int NetAddress::getNetmask(long address) {

    return (int)((address >> 48) & 0xFFFF);

}

sockaddr_in NetAddress::getInetAddress(long address) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = (uint16_t)getPort(address);
    inet_addr.sin_addr.s_addr = (uint32_t)getIP(address);
    return inet_addr;

}

bool NetAddress::isLoopback(long address) {

    return  (((address) & IPADDRESS_MASK) == LOOPBACK_ADDRESS);
}

std::vector<long> NetAddress::getAddressList(long address) {

    std::vector<long> list;

    int netmask = getNetmask(address);

    long ownIP = getIP(address);

    if (NetAddress::isLoopback(address)) {

        for (int i = 0; i < LOOPBACK_RANGE; i++) {

            long destAddress = parseAddress(ownIP, DEFAULT_PORT + i, netmask);

            if (destAddress != address) {

                list.push_back(destAddress);

            }

        }

    } else {

        long range = 1 << (32 - netmask);

        long startIP = (1 >> netmask) & ownIP + 1;

        for (int i = 0; i < range; i++) {

            if (startIP != ownIP) {

                long destAddress = parseAddress(startIP, DEFAULT_PORT, netmask);

                list.push_back(destAddress);

            }

            startIP++;
        }

    }

    return list;
}

int NetAddress::address2prefix(long address) {

    int i = 0;
    uint32_t ip = (uint32_t) getIP(address);
    while(ip > 0) {
        ip = ip >> 1;
        i++;
    }

    return i;
}
