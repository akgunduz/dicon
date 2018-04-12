//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Util.h"
#include "ArchTypes.h"

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

std::vector<std::string> Util::getFileList(const char *path, const char* filter) {

    std::string file;
    std::vector<std::string> fileList;

    struct dirent *ent;

    DIR *dir = opendir(path);
    if (dir == nullptr) {
        LOG_E("Directory : %s could not opened\n err: %d", path, errno);
        return fileList;
    }

    while((ent = readdir(dir)) != nullptr) {

        if (ent->d_type != DT_REG) {
            continue;
        }

        if (strncmp(ent->d_name, filter, strlen(filter)) != 0) {
            continue;
        }

        file = std::string(path) + "/" + ent->d_name;
        fileList.push_back(file);
    }

    return fileList;
}

std::vector<std::string> Util::getDirList(const char *path, const char* filter) {

    std::string file;
    std::vector<std::string> fileList;

    struct dirent *ent;

    DIR *dir = opendir(path);
    if (dir == nullptr) {
        LOG_E("Directory : %s could not opened\n err: %d", path, errno);
        return fileList;
    }

    while((ent = readdir(dir)) != nullptr) {

        if (ent->d_type != DT_DIR) {
            continue;
        }

        if (strncmp(ent->d_name, filter, strlen(filter)) != 0) {
            continue;
        }

   //     file = std::string(path) + "/" + ent->d_name;
        fileList.push_back(ent->d_name);
    }

    return fileList;
}



bool Util::isMulticast() {

#ifdef MULTICAST_ENABLED
    return true;
#else
    return false;
#endif
}

std::string Util::mixPath(const char *path1, const char *path2) {
    return std::string(path1) + "/" + path2;
}

bool Util::checkPath(const char *path1, const char *path2, bool dir) {
    std::string final = std::string(path1) + "/" + path2;
    if (access(final.c_str(), F_OK ) == -1) {
        if (dir) {
            mkPath(final.c_str());
        }
        return false;
    }
    return true;
}

std::string Util::absPath(COMPONENT host, const char *path2) {
    return std::string(ComponentTypes::getRootPath(host)) + "/" + path2;
}

void Util::cleanup() {

    DIR *unixdir = opendir(UNIXSOCKET_PATH);
    if (!unixdir) {
        printf("Can not open unix socket path!!!");
        return;
    }

    dirent *entry;
    char path[255];

    while ((entry = readdir(unixdir)) != NULL) {

        if (strncmp(entry->d_name, UNIXSOCKET_FILE_PREFIX, strlen(UNIXSOCKET_FILE_PREFIX)) == 0) {
            sprintf(path, "%s%s", UNIXSOCKET_PATH, entry->d_name);
            unlink(path);
        }
    }

    closedir(unixdir);
}
