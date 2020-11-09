//
// Created by Haluk AKGUNDUZ on 04.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_UTILNET_H
#define DICON_UTILNET_H

#include <string>

#include "Address.h"
#include "Platform.h"

#define UNIXSOCKET_FILE_PREFIX "dicon_"
#define UNIXSOCKET_FILE_SUFFIX ".sock"

class UtilNet {

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

#endif //DICON_UTILNET_H
