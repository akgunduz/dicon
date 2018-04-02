//
// Created by akgunduz on 02.04.2018.
//

#ifndef BANKOR_INTERFACETYPES_H
#define BANKOR_INTERFACETYPES_H

#include "Common.h"

enum INTERFACE {
    INTERFACE_NET,
    INTERFACE_UNIXSOCKET,
    INTERFACE_MAX
};

class Device;

typedef std::vector<long> (*fGetAddressList)(Device*);

class InterfaceTypes {
public:
    static fGetAddressList getAddressList(INTERFACE);
    static std::string getAddressString(long);
};


#endif //BANKOR_INTERFACETYPES_H
