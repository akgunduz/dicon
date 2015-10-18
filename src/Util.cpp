//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Util.h"

std::string Util::hex2str(const uint8_t *in, int len) {
	static const char* const lut = "0123456789ABCDEF";
	std::string output;
	output.reserve((uint32_t)2 * len);
	for (size_t i = 0; i < len; ++i) {
		output.push_back(lut[in[i] >> 4]);
		output.push_back(lut[in[i] & 15]);
	}
	return output;
}

bool Util::str2hex(uint8_t *out, const char *in, uint32_t len) {
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

void Util::mkPath(const char *path) {
    char tmp[PATH_MAX];
    char *p = NULL;

    const char *pos = strrchr(path, '/');

    snprintf(tmp, pos - path + 1, "%s", path);

    for(p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}

short Util::getID() {
	return ARCH_OSX;
}

const char* Util::getIDDir(enum ARCHIDS id) {
	return sArchs[id];
}
