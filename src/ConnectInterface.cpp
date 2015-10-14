//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "ConnectInterface.h"
#include "NetAddress.h"

std::vector<ConnectInterface> ConnectInterface::interfaceList;
bool initialized = false;

std::vector<ConnectInterface> ConnectInterface::getInterfaces() {

    if (initialized) {
        return interfaceList;
    }

    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return interfaceList;
    }

    for (loop = ifAddrStruct; loop != NULL; loop = loop->ifa_next) {

        if (loop->ifa_addr->sa_family == AF_INET) { // check it is IP4

            if (strncmp(loop->ifa_name, "et", 2) == 0 ||
                strncmp(loop->ifa_name, "en", 2) == 0 ||
                strncmp(loop->ifa_name, "br", 2) == 0 ||
                strncmp(loop->ifa_name, "lo", 2) == 0) {

                interfaceList.push_back(ConnectInterface(loop->ifa_name,
                    ((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr,
                    NetAddress::address2prefix(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)));

            }
        }
    };

    interfaceList.push_back(ConnectInterface("us", INTERFACE_UNIXSOCKET));
    interfaceList.push_back(ConnectInterface("pp", INTERFACE_PIPE));

    freeifaddrs(ifAddrStruct);

    initialized = true;

    return interfaceList;
}


int ConnectInterface::getCount() {

    return (int)interfaceList.size();

}

ConnectInterface* ConnectInterface::getInterface(int index) {

    return &interfaceList[index >= interfaceList.size() ?
                          interfaceList.size() - 1 : index];

}

std::string ConnectInterface::getName(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ?
                                       interfaceList.size() - 1 : index];

    return ci->name;
}

INTERFACES ConnectInterface::getType(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ? interfaceList.size() - 1 : index];

    return ci->type;

}

long ConnectInterface::getAddress(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ?
                                       interfaceList.size() - 1 : index];

    return ci->address;
}

long ConnectInterface::getHelper(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ?
                                          interfaceList.size() - 1 : index];

    return ci->helper;
}

/*
bool ConnectInterface::isInterfaceLoopback(uint32_t index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ? interfaceList.size() - 1 : index];

    return strncmp(ci->name.c_str(), "lo", 2) == 0;

}

long ConnectInterface::getNetInterfaceNetwork(long refAddress) {

    uint32_t refIPAddress = (uint32_t)(refAddress & IPADDRESS_MASK);

    for (uint32_t i = 0; i < interfaceList.size(); i++) {

        ConnectInterface *ci = &interfaceList[i];

        if (ci->type != INTERFACE_NET) {
            continue;
        }

        if (refIPAddress == ci->ipAddress) {

            return ci->ipAddress & ci->netmask;

        }
    }

    return 0;
}

long ConnInterface::getNetInterfaceInfo(int index, long &startIP) {

    ConnInterface *ci = &interfaceList[index >= interfaceList.size() ? interfaceList.size() - 1 : index];

    if (ci->type != INTERFACE_NET) {
        return 0;
    }

    long network = ci->ipAddress & ci->netmask;

    startIP = network + 1;

    return (long) ~ci->netmask - 1;
}

long ConnInterface::getNetInterfaceInfo(long refAddress, long &startIP) {

    long refIPAddress = (long)(refAddress & IPADDRESS_MASK);

    for (uint32_t i = 0; i < interfaceList.size(); i++) {

        ConnInterface *ci = &interfaceList[i];

        if (ci->type != INTERFACE_NET) {
            continue;
        }

        if (refIPAddress == ci->ipAddress) {

            long network = ci->ipAddress & ci->netmask;

            startIP = network + 1;

            return ~ci->netmask - 1;
        }
    }

    startIP = 0;

    return 0;
}
*/

