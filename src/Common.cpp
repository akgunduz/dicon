//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <ifaddrs.h>
#include "Common.h"
#include "Tools.h"

std::vector<ConnInterface> ConnInterface::mInterfaceList;

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


uint32_t ConnInterface::initInterfaces() {

	struct ifaddrs * ifAddrStruct = nullptr;
	struct ifaddrs * loop = nullptr;

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

				mInterfaceList.push_back(ConnInterface(loop->ifa_name,
						ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
						ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr)));

			}
		}
	};

	mInterfaceList.push_back(ConnInterface("us", INTERFACE_UNIXSOCKET));
	mInterfaceList.push_back(ConnInterface("pp", INTERFACE_PIPE));

	freeifaddrs(ifAddrStruct);

	return (uint32_t)mInterfaceList.size();
}


uint32_t ConnInterface::getInterfaceCount() {

	return (uint32_t)mInterfaceList.size();

}

ConnInterface* ConnInterface::getInterface(uint32_t index) {

	return &mInterfaceList[index >= mInterfaceList.size() ?
			mInterfaceList.size() - 1 : index];

}

std::string ConnInterface::getInterfaceName(uint32_t index) {

	ConnInterface *ci = &mInterfaceList[index >= mInterfaceList.size() ?
			mInterfaceList.size() - 1 : index];

	return ci->mName;
}

INTERFACES ConnInterface::getInterfaceType(uint32_t index) {

	ConnInterface *ci = &mInterfaceList[index >= mInterfaceList.size() ?
			mInterfaceList.size() - 1 : index];

	return ci->mType;

}

bool ConnInterface::isInterfaceLoopback(uint32_t index) {

	ConnInterface *ci = &mInterfaceList[index >= mInterfaceList.size() ?
			mInterfaceList.size() - 1 : index];

	return strncmp(ci->mName.c_str(), "lo", 2) == 0;

}

uint32_t ConnInterface::getNetInterfaceNetwork(long refAddress) {

	uint32_t refIPAddress = (uint32_t)(refAddress & IPADDRESS_MASK);

	for (uint32_t i = 0; i < mInterfaceList.size(); i++) {

		ConnInterface *ci = &mInterfaceList[i];

		if (ci->mType != INTERFACE_NET) {
			continue;
		}

		if (refIPAddress == ci->mIPAddress) {

			return ci->mIPAddress & ci->mNetmask;

		}
	}

	return 0;
}

uint32_t ConnInterface::getNetInterfaceInfo(uint32_t index, uint32_t &startIP) {

	ConnInterface *ci = &mInterfaceList[index >= mInterfaceList.size() ?
			mInterfaceList.size() - 1 : index];

	if (ci->mType != INTERFACE_NET) {
		return 0;
	}

	uint32_t network = ci->mIPAddress & ci->mNetmask;

	startIP = network + 1;

	return (uint32_t) ~ci->mNetmask - 1;
}

uint32_t ConnInterface::getNetInterfaceInfo(long refAddress, uint32_t &startIP) {

	uint32_t refIPAddress = (uint32_t)(refAddress & IPADDRESS_MASK);

	for (uint32_t i = 0; i < mInterfaceList.size(); i++) {

		ConnInterface *ci = &mInterfaceList[i];

		if (ci->mType != INTERFACE_NET) {
			continue;
		}

		if (refIPAddress == ci->mIPAddress) {

			uint32_t network = ci->mIPAddress & ci->mNetmask;

			startIP = network + 1;

			return (uint32_t) ~ci->mNetmask - 1;
		}
	}

	startIP = 0;

	return 0;
}

uint32_t ConnInterface::getNetInterfaceAddress(uint32_t index) {

	ConnInterface *ci = &mInterfaceList[index >= mInterfaceList.size() ?
			mInterfaceList.size() - 1 : index];

	if (ci->mType != INTERFACE_NET) {
		return 0;
	}

	return ci->mIPAddress;
}
