//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Common.h"

std::vector<ConnInterface> ConnInterface::interfaceList;

const char* sStates[] = {
		"IDLE",
		"BUSY",
		"PREBUSY"
};

const char* sParamTypes[] = {
		"LONG",
		"DOUBLE",
		"STRING",
};

const char* sInterfaces[] = {
		"TCP",
		"UnixSocket",
		"Pipe",
};


int ConnInterface::initInterfaces() {

	struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return 0;
    }

	for (loop = ifAddrStruct; loop != NULL; loop = loop->ifa_next) {

		if (loop->ifa_addr->sa_family == AF_INET) { // check it is IP4

			if (strncmp(loop->ifa_name, "et", 2) == 0 ||
					strncmp(loop->ifa_name, "en", 2) == 0 ||
					strncmp(loop->ifa_name, "br", 2) == 0 ||
					strncmp(loop->ifa_name, "lo", 2) == 0) {

				interfaceList.push_back(ConnInterface(loop->ifa_name,
						ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
						ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)));

			}
		}
	};

	interfaceList.push_back(ConnInterface("us", INTERFACE_UNIXSOCKET));
	interfaceList.push_back(ConnInterface("pp", INTERFACE_PIPE));

    freeifaddrs(ifAddrStruct);

	return (int)interfaceList.size();
}


int ConnInterface::getInterfaceCount() {

	return (int)interfaceList.size();

}

ConnInterface* ConnInterface::getInterface(uint32_t index) {

	return &interfaceList[index >= interfaceList.size() ?
						   interfaceList.size() - 1 : index];

}

std::string ConnInterface::getInterfaceName(uint32_t index) {

	ConnInterface *ci = &interfaceList[index >= interfaceList.size() ?
									   interfaceList.size() - 1 : index];

	return ci->name;
}

INTERFACES ConnInterface::getInterfaceType(uint32_t index) {

	ConnInterface *ci = &interfaceList[index >= interfaceList.size() ? interfaceList.size() - 1 : index];

	return ci->type;

}

bool ConnInterface::isInterfaceLoopback(uint32_t index) {

	ConnInterface *ci = &interfaceList[index >= interfaceList.size() ? interfaceList.size() - 1 : index];

	return strncmp(ci->name.c_str(), "lo", 2) == 0;

}

long ConnInterface::getNetInterfaceNetwork(long refAddress) {

	uint32_t refIPAddress = (uint32_t)(refAddress & IPADDRESS_MASK);

	for (uint32_t i = 0; i < interfaceList.size(); i++) {

		ConnInterface *ci = &interfaceList[i];

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

long ConnInterface::getNetInterfaceAddress(int index) {

	ConnInterface *ci = &interfaceList[index >= interfaceList.size() ?
			interfaceList.size() - 1 : index];

	if (ci->type != INTERFACE_NET) {
		return 0;
	}

	return ci->ipAddress;
}
