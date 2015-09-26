//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Common_H_
#define __Common_H_

#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <json-c/json.h>

#include <iostream>
#include <iosfwd>
#include <fstream>
#include <string>

#include <assert.h>

#include <cstdarg>
#include <cstring>

#include <list>
#include <vector>
#include <queue>
#include <map>

#include <complex>
#include <stdexcept>

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdexcept>
#include <ifaddrs.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef __APPLE__
#include <CommonCrypto/CommonDigest.h>
#include "Common_apple.h"

#ifdef MD5_DIGEST_LENGTH
#undef MD5_DIGEST_LENGTH
#endif

#define MD5_Init            CC_MD5_Init
#define MD5_Update          CC_MD5_Update
#define MD5_Final           CC_MD5_Final
#define MD5_DIGEST_LENGTH   CC_MD5_DIGEST_LENGTH
#define MD5_CTX             CC_MD5_CTX

#else
#include <openssl/md5.h>
#include "Common_linux.h"
#endif

#ifndef nullptr
#define nullptr NULL
#endif

#define RULE_FILE "Rule.json"

#define SHUTDOWN_NOTIFIER 'Q'
#define LOOPBACK_RANGE 256
#define LOOPBACK_ADDRESS 0x7F000001
#define IPADDRESS_MASK 0xFFFFFFFF
#define IS_LOOPBACK(address) (((address) & IPADDRESS_MASK) == LOOPBACK_ADDRESS)
#define GEN_ADDRESS(ip, port) ((((long)ip) & IPADDRESS_MASK) | (((long)port) << 32))

enum HOST {
	HOST_DISTRIBUTOR,
	HOST_COLLECTOR,
	HOST_CLIENT
};

enum INTERFACES {
	INTERFACE_NET,
	INTERFACE_UNIXSOCKET,
	INTERFACE_PIPE,
	INTERFACE_MAX
};

enum STATES {
	IDLE,
	BUSY,
	PREBUSY,
	REMOVE
};

enum UI_UPDATE {
	UI_UPDATE_LOG,

	UI_UPDATE_DIST_ADDRESS,
	UI_UPDATE_DIST_COLL_LIST,
	UI_UPDATE_DIST_CLIENT_LIST,
	UI_UPDATE_DIST_BACKUP,
	UI_UPDATE_DIST_LOG,

	UI_UPDATE_COLL_ADDRESS,
	UI_UPDATE_COLL_ATT_DIST_ADDRESS,
	UI_UPDATE_COLL_ATT_CLIENT_ADDRESS,
	UI_UPDATE_COLL_FILE_LIST,
	UI_UPDATE_COLL_PARAM_LIST,
	UI_UPDATE_COLL_EXEC_LIST,
	UI_UPDATE_COLL_LOG,

	UI_UPDATE_CLIENT_ADDRESS,
	UI_UPDATE_CLIENT_STATE,
	UI_UPDATE_CLIENT_ATT_COLL_ADDRESS,
	UI_UPDATE_CLIENT_FILE_LIST,
	UI_UPDATE_CLIENT_PARAM_LIST,
	UI_UPDATE_CLIENT_EXEC_LIST,
	UI_UPDATE_CLIENT_LOG,

	UI_UPDATE_MAX
};

class ConnInterface {
public:
	std::string mName;
	INTERFACES mType;
	uint32_t mIPAddress;
	uint32_t mNetmask;
	static std::vector<ConnInterface>mInterfaceList;

	ConnInterface(std::string name, INTERFACES type) :
			mName(name), mType(type), mIPAddress(0), mNetmask(0)  {};
	ConnInterface(std::string name, uint32_t ipAddress, uint32_t netmask) :
			mName(name), mType(INTERFACE_NET), mIPAddress(ipAddress), mNetmask(netmask) {};

	static ConnInterface* getInterface(uint32_t);
	static std::string getInterfaceName(uint32_t);
	static INTERFACES getInterfaceType(uint32_t);
	static bool isInterfaceLoopback(uint32_t);

	static uint32_t initInterfaces();
	static uint32_t getInterfaceCount();
	static uint32_t getNetInterfaceAddress(uint32_t);
	static uint32_t getNetInterfaceNetwork(long);
	static uint32_t getNetInterfaceInfo(uint32_t, uint32_t&);
	static uint32_t getNetInterfaceInfo(long, uint32_t&);
};

class EventData {

public:
	uint64_t data64_1;
	uint64_t data64_2;
	std::string dataStr;

	EventData(uint64_t data) : data64_1(data) {}
	EventData(uint64_t data, uint64_t data2) : data64_1(data), data64_2(data2) {}
	EventData(uint64_t data, const char *data2) : data64_1(data), dataStr(data2) {}
	EventData(uint64_t data, uint64_t data2, const char *data3) : data64_1(data), data64_2(data2), dataStr(data3) {}
	EventData(const char *data) : dataStr(data) {}
};

extern const char* sStates[];
extern const char* sParamTypes[];
extern const char* sInterfaces[];

#endif
