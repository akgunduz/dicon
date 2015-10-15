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
                    ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                    NetAddress::address2prefix(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr))));

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