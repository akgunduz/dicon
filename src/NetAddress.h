//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_NETADDRESS_H
#define BANKOR_NETADDRESS_H

#include "Common.h"
#include "Address.h"

#define LOOPBACK_ADDRESS 0x7F000001
#define IPADDRESS_MASK 0xFFFFFFFF

class NetAddress {

public:
    static std::string getString(long);
    static long getIP(long);
    static std::string getIPstr(long);
    static int getPort(long);
    static sockaddr_in getInetAddress(long);
    static bool isLoopback(long);
    static long parseIP(const std::string &ip);
    static long parseAddress(long ip, int port);
};


#endif //BANKOR_NETADDRESS_H
