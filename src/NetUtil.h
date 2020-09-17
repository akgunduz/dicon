//
// Created by Haluk AKGUNDUZ on 4.08.2020.
//

#ifndef DICON_NETUTIL_H
#define DICON_NETUTIL_H

#include "Address.h"

#define UNIXSOCKET_FILE_PREFIX "dicon_"
#define UNIXSOCKET_FILE_SUFFIX ".sock"

class NetUtil {

public:

    static std::string getIPString(uint32_t);
    static std::string getIPString(BaseAddress&);
    static std::string getIPString(const sockaddr_in *);
    static long parseIPAddress(const std::string&);
    static sockaddr_in getInetAddressByAddress(TypeAddress&);
    static sockaddr_in getInetAddressByPort(int);
    static ip_mreq getInetMulticastAddress(TypeAddress&, TypeAddress&);
    static BaseAddress parseIPPortString(const std::string&);
    static std::string getIPPortString(BaseAddress&);

    static sockaddr_un getUnixAddress(TypeAddress&);
    static std::string getUnixString(BaseAddress&);
    static void cleanUnixPath();
};

#endif //DICON_NETUTIL_H
