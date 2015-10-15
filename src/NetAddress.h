//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_NETADDRESS_H
#define BANKOR_NETADDRESS_H

#include "Common.h"
#include "Address.h"

#define LOOPBACK_RANGE 256
#define LOOPBACK_ADDRESS 0x0100007F
#define IPADDRESS_MASK 0xFFFFFFFF
#define DEFAULT_PORT 61001

class NetAddress {

public:
    static std::string getString(long);
    static long getIP(long);
    static std::string getIPstr(long);
    static int getPort(long);
    static int getNetmask(long);
    static sockaddr_in getInetAddress(long);
    static bool isLoopback(long);
    static long parseAddress(long ip, int port, int netmask);
    static std::vector<long> getAddressList(long address);
    static int address2prefix(long address);
};


#endif //BANKOR_NETADDRESS_H
