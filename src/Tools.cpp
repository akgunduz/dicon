//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <arpa/inet.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Tools.h"

std::string Tools::getIPStr(long ip) {
	struct in_addr addr;
	addr.s_addr = htonl((uint32_t)ip);
	char cIP[INET_ADDRSTRLEN];

	const char *dst = inet_ntop(AF_INET, &addr, cIP, INET_ADDRSTRLEN);
	if (!dst) {
		LOG_E("Ip is invalid");
		return "";
	}

	std::string sIP = cIP;
	sIP += ":" + std::to_string(ip >> 32);
	return sIP;
}

std::string Tools::hex2str(const uint8_t *in, int len) {
	static const char* const lut = "0123456789ABCDEF";
	std::string output;
	output.reserve((uint32_t)2 * len);
	for (size_t i = 0; i < len; ++i) {
		output.push_back(lut[in[i] >> 4]);
		output.push_back(lut[in[i] & 15]);
	}
	return output;
}

bool Tools::str2hex(uint8_t *out, const char *in, uint32_t len) {
	static const char* const lut = "0123456789ABCDEF";
	if (len & 1) {
		return false;
	};

	for (size_t i = 0; i < len; i++)
	{
		char a = (char)toupper(in[i * 2]);
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) {
			return false;
		};

		char b = (char)toupper(in[i * 2 + 1]);
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b) {
			return false;
		};

		out[i] = (unsigned char)(((p - lut) << 4) | (q - lut));
	}

	return true;
}

std::string Tools::getMD5Str(uint8_t *md5) {
	return hex2str(md5, MD5_DIGEST_LENGTH);
}

void Tools::mkpath(const char *path) {
        char tmp[PATH_MAX];
        char *p = NULL;
        size_t len;
        
        const char *pos = strrchr(path, '/');

        snprintf(tmp, pos - path + 1, "%s", path);

        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}

std::string Tools::getAddressStr(long address) {
	INTERFACES interfaces = getInterface(address);
	if (interfaces == INTERFACE_NET) {
		return getIPStr(address);
	}
	return std::to_string(address);
}


INTERFACES Tools::getInterface(long address) {

	if (address > 0xFFFFFF) {
		return INTERFACE_NET;

	} else if (address > 1000) {
		return INTERFACE_UNIXSOCKET;

	}

	return INTERFACE_PIPE;

}

void DiffTime::start() {

	struct timeval st;
	int res = gettimeofday(&st, nullptr);
	if (res != 0) {
		return;
	}

	tStart = st.tv_sec + (double)st.tv_usec / 1000000;

	initiated = true;
}

double DiffTime::stop() {

	if (!initiated) {
		return 0;
	}

	struct timeval st;
	int res = gettimeofday(&st, nullptr);
	if (res != 0) {
		return 0;
	}

	tStop = st.tv_sec + (double)st.tv_usec / 1000000;

	return tStop - tStart;

}

void DiffTime::reset() {

	initiated = false;

}

bool DiffTime::isInitiated() {

	return initiated;

}
