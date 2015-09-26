//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include <arpa/inet.h>
#include "NetAddress.h"
#include "Log.h"

NetAddress::NetAddress(long address) {

    set(address);
}

NetAddress::NetAddress(long ip, int port) {

    set(ip, port);
}

NetAddress::NetAddress(const std::string &ip, int port) {

    set(ip, port);
}


void NetAddress::set(long address) {

    Address::set(address);

    init();
}

void NetAddress::set(long ip, int port) {

    Address::set(((long)port << 40) | ip);

    init();
}

void NetAddress::set(const std::string &ip, int port) {

    long newIP = parseIP(ip);

    Address::set(((long)port << 40) | newIP);

    init();
}

void NetAddress::init() {

    debug_ip = getIP();
    debug_port = getPort();

    bzero((char *) &inet_addr, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons((uint16_t)getPort());
    inet_addr.sin_addr.s_addr = htonl(getIP());
}

std::string NetAddress::getString() {

    return getIPstr() + ":" + std::to_string(getPort());

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

long NetAddress::getIP() {

    return address & 0xFFFFFFFFL;
}

void NetAddress::setIP(const std::string &ip) {

    address = ((long)getPort() << 40) | parseIP(ip);

    this->debug_ip = getIP();

}

std::string NetAddress::getIPstr() {

    struct in_addr addr;
    addr.s_addr = htonl((uint32_t)getIP());
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        LOG_E("Ip is invalid");
        return "";
    }

    return std::string(cIP);
}

int NetAddress::getPort() {

    return (int)((address >> 40) & 0xFFFF);

}

void NetAddress::setPort(int port) {

    address = ((long)port << 40) | getIP();

    this->debug_port = port;

}

INTERFACES NetAddress::getInterface() {

    return INTERFACE_NET;
}

sockaddr_in NetAddress::getInetAddress() {

    return inet_addr;

}

bool NetAddress::isLoopback() {

    return  (((address) & IPADDRESS_MASK) == LOOPBACK_ADDRESS);
}
