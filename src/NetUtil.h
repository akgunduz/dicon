//
// Created by Haluk AKGUNDUZ on 4.08.2020.
//

#ifndef DICON_NETUTIL_H
#define DICON_NETUTIL_H

#include "Address.h"

class NetUtil {

public:

    static std::string getIPString(BaseAddress&);
    static long parseIPAddress(const std::string&);
    static sockaddr_in getInetAddressByAddress(Address&);
    static sockaddr_in getInetAddressByPort(int);
    static ip_mreq getInetMulticastAddress(Address&, uint32_t);
    static BaseAddress parseIPPortString(std::string);
    static std::string getIPPortString(BaseAddress&);

    static sockaddr_un getUnixAddress(Address&);
    static std::string getUnixString(BaseAddress&);
};

#endif //DICON_NETUTIL_H
