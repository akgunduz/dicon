//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "NetAddress.h"
#include "Log.h"

std::string NetAddress::getString(long address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%ld", getIPstr(address).c_str(), address);
    return std::string(sAddress);
}

long NetAddress::parseIP(const std::string &ip) {

    struct in_addr addr;

    int res = inet_pton(AF_INET, ip.c_str(), &addr);
    if (!res) {
        LOG_E("Ip is invalid");
        return 0;
    }

    return ntohl(addr.s_addr);
}

long NetAddress::parseAddress(long ip, int port) {

    return ((long)port << 40) | ip;

}

long NetAddress::getIP(long address) {

    return address & 0xFFFFFFFFL;
}

std::string NetAddress::getIPstr(long address) {

    struct in_addr addr;
    addr.s_addr = htonl((uint32_t)getIP(address));
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        LOG_E("Ip is invalid");
        return "";
    }

    return std::string(cIP);
}

int NetAddress::getPort(long address) {

    return (int)((address >> 40) & 0xFFFF);

}

sockaddr_in NetAddress::getInetAddress(long address) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons((uint16_t)getPort(address));
    inet_addr.sin_addr.s_addr = htonl((u_long)getIP(address));
    return inet_addr;

}

bool NetAddress::isLoopback(long address) {

    return  (((address) & IPADDRESS_MASK) == LOOPBACK_ADDRESS);
}
