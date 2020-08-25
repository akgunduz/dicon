//
// Created by Haluk AKGUNDUZ on 4.08.2020.
//

#include "NetUtil.h"

std::string NetUtil::getIPPortString(BaseAddress& address) {

    char sAddress[50];
    sprintf(sAddress, "%s:%u", getIPString(address).c_str(), address.port);
    return std::string(sAddress);
}

std::string NetUtil::getIPString(BaseAddress& address) {

    struct in_addr addr{};
    addr.s_addr = htonl(address.base);
    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
    if (!dst) {
        return "";
    }

    return std::string(cIP);
}

std::string NetUtil::getIPString(const sockaddr_in *address) {

    char cIP[INET_ADDRSTRLEN];

    const char *dst = inet_ntop(AF_INET, &(address->sin_addr), cIP, INET_ADDRSTRLEN);
    if (!dst) {
        return "";
    }

    return std::string(cIP);
}

long NetUtil::parseIPAddress(const std::string& address) {

    struct in_addr addr{};

    int res = inet_pton(AF_INET, address.c_str(), &addr);
    if (res <= 0) {
        return 0;
    }

    return ntohl(addr.s_addr);
}

BaseAddress NetUtil::parseIPPortString(std::string address) {

    long pos = address.find(':');

    std::string sIP = address.substr(0, pos);

    std::string sPort = address.substr(pos + 1, std::string::npos);

    long ip = parseIPAddress(sIP);
    int port = atoi(sPort.c_str());

    return BaseAddress(ip, port);
}


sockaddr_in NetUtil::getInetAddressByAddress(Address& address) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(address.get().port);
    inet_addr.sin_addr.s_addr = htonl(address.get().base);
    return inet_addr;
}

sockaddr_in NetUtil::getInetAddressByPort(int port) {

    sockaddr_in inet_addr;
    memset((char *) &inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(port);
    inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return inet_addr;
}

ip_mreq NetUtil::getInetMulticastAddress(Address& address, uint32_t multicastAddress) {

    ip_mreq imreq;
    memset((char *) &imreq, 0, sizeof(imreq));

    imreq.imr_multiaddr.s_addr = htonl(multicastAddress);
    imreq.imr_interface.s_addr = htonl(address.get().base);
    return imreq;
}

sockaddr_un NetUtil::getUnixAddress(Address& address) {

    sockaddr_un unix_addr{};
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s%s%u%s",
            std::filesystem::temp_directory_path().string().c_str(),
            UNIXSOCKET_FILE_PREFIX,
            address.get().base,
            UNIXSOCKET_FILE_SUFFIX);
    return unix_addr;
}

std::string NetUtil::getUnixString(BaseAddress& address) {

    char sAddress[50];
    sprintf(sAddress, "%u", address.base);
    return std::string(sAddress);
}
