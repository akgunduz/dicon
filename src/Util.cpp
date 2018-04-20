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

bool Util::checkPath(const char *root, const char *path, bool dir) {

    std::string final = std::string(root) + "/" + path;
    if (access(final.c_str(), F_OK ) == -1) {
        if (dir) {
            mkPath(final.c_str());
        }
        return false;
    }
    return true;
}

bool Util::checkPath(const char *root, const char* jobDir, const char* fileName, bool dir) {

    std::string final = std::string(root) + "/" + jobDir + "/" + fileName;
    if (access(final.c_str(), F_OK ) == -1) {
        if (dir) {
            mkPath(final.c_str());
        }
        return false;
    }
    return true;
}

std::string Util::parsePath(COMPONENT host, const char *str) {

    unsigned long len = strlen(ROOT_SIGN);
    unsigned long pos = 0;

    std::string newstr = str;

    do {

        pos = newstr.find(ROOT_SIGN, pos);

        if (pos == std::string::npos) {
            break;
        }

        newstr.replace(pos, len, ComponentTypes::getRootPath(host));

    } while(true);

    return newstr;
}

std::string Util::getAbsPath(COMPONENT host, const char *path2) {

    return std::string(ComponentTypes::getRootPath(host)) + "/" + path2;
}

std::string Util::getAbsRefPath(COMPONENT host, const char* jobDir, const char* fileName) {

    return getAbsPath(host, getRefPath(host, jobDir, fileName).c_str());
}

std::string Util::getAbsMD5Path(COMPONENT host, const char* jobDir, const char* fileName) {

    return getAbsPath(host, getMD5Path(host, jobDir, fileName).c_str());
}

std::string Util::getRefPath(COMPONENT host, const char* jobDir, const char* fileName) {

    return getPath(host, jobDir, fileName, false);
}

std::string Util::getMD5Path(COMPONENT host, const char* jobDir, const char* fileName) {

    return getPath(host, jobDir, fileName, true);
}

std::string Util::getPath(COMPONENT host, const char* jobDir, const char* fileName, bool type) {

    char format[PATH_MAX];
    char path[PATH_MAX];

    !type ? strcpy(format, "%s/%s") : strcpy(format, "%s/md5/%s.md5");
    sprintf(path, format, jobDir, fileName);

    checkPath(ComponentTypes::getRootPath(host), path, true);

    return std::string(path);
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
