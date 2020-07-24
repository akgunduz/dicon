#include <climits>
//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <miniz/miniz.h>
#include "Util.h"

std::string Util::hex2str(const uint8_t *in, int len) {
    static const char *const lut = "0123456789ABCDEF";
    std::string output;
    output.reserve((uint32_t) 2 * len);
    for (size_t i = 0; i < len; ++i) {
        output.push_back(lut[in[i] >> 4]);
        output.push_back(lut[in[i] & 15]);
    }
    return output;
}

bool Util::str2hex(uint8_t *out, const char *in, uint32_t len) {
    static const char *const lut = "0123456789ABCDEF";
    if (len & 1) {
        return false;
    };

    for (size_t i = 0; i < len; i++) {
        char a = (char) toupper(in[i * 2]);
        const char *p = std::lower_bound(lut, lut + 16, a);
        if (*p != a) {
            return false;
        };

        char b = (char) toupper(in[i * 2 + 1]);
        const char *q = std::lower_bound(lut, lut + 16, b);
        if (*q != b) {
            return false;
        };

        out[i] = (unsigned char) (((p - lut) << 4) | (q - lut));
    }

    return true;
}

void Util::mkPath(const char *path) {
    char tmp[PATH_MAX];
    char *p = nullptr;

    const char *pos = strrchr(path, '/');
    if (pos == nullptr) {
        return;
    }

    snprintf(tmp, pos - path + 1, "%s", path);

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}

std::vector<std::string> Util::getFileList(const char *path, const char *filter) {

    std::string file;
    std::vector<std::string> fileList;

    struct dirent *ent;

    DIR *dir = opendir(path);
    if (dir == nullptr) {
        //LOG_E("Directory : %s could not opened\n err: %d", path, errno);
        return fileList;
    }

    while ((ent = readdir(dir)) != nullptr) {

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

std::vector<std::string> Util::getDirList(const char *path, const char *filter) {

    std::string file;
    std::vector<std::string> fileList;

    struct dirent *ent;

    DIR *dir = opendir(path);
    if (dir == nullptr) {
        //LOG_E("Directory : %s could not opened\n err: %d", path, errno);
        return fileList;
    }

    while ((ent = readdir(dir)) != nullptr) {

        if (ent->d_type != DT_DIR) {
            continue;
        }

        if (strncmp(ent->d_name, filter, strlen(filter)) != 0) {
            continue;
        }

        fileList.emplace_back(ent->d_name);
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

void Util::removePath(const char *path) {

    struct dirent *entry;
    char newPath[PATH_MAX];

    DIR *dir = opendir(path);
    if (dir == nullptr) {
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            snprintf(newPath, (size_t) PATH_MAX, "%s/%s", path, entry->d_name);
            if (entry->d_type == DT_DIR) {
                removePath(newPath);
            } else {
                unlink(newPath);
            }

        }

    }
    closedir(dir);

    rmdir(path);
}

bool Util::checkPath(const char *path, bool dir) {

    if (access(path, F_OK) == -1) {
        if (dir) {
            mkPath(path);
            return true;
        }
        return false;
    }
    return true;
}

bool Util::checkPath(const char *root, const char *path, bool dir) {

    std::string final = std::string(root) + "/" + path;
    if (access(final.c_str(), F_OK) == -1) {
        if (dir) {
            mkPath(final.c_str());
            return true;
        }
        return false;
    }
    return true;
}

bool Util::checkPath(const char *rootPath, long jobID, const char *fileName, bool dir) {

    std::string final = std::string(rootPath) + "/" + std::to_string(jobID) + "/" + fileName;
    if (access(final.c_str(), F_OK) == -1) {
        if (dir) {
            mkPath(final.c_str());
            return true;
        }
        return false;
    }
    return true;
}

std::string Util::parsePath(const char *rootPath, const char *str) {

    unsigned long len = strlen(ROOT_SIGN);
    unsigned long pos = 0;

    std::string newstr = str;

    do {

        pos = newstr.find(ROOT_SIGN, pos);

        if (pos == std::string::npos) {
            break;
        }

        newstr.replace(pos, len, rootPath);

    } while (true);

    return newstr;
}

std::string Util::getAbsPath(const char *rootPath, const char *path2) {

    return std::string(rootPath) + "/" + path2;
}

std::string Util::getAbsRefPath(const char *rootPath, long jobID, const char *fileName) {

    return getAbsPath(rootPath, getRefPath(rootPath, jobID, fileName).c_str());
}

std::string Util::getRefPath(const char *rootPath, long jobID, const char *fileName) {

    return getPath(rootPath, jobID, fileName, false);
}

std::string Util::getPath(const char *rootPath, long jobID, const char *fileName, bool type) {

    char format[PATH_MAX];
    char path[PATH_MAX];

    !type ? strcpy(format, "%d/%s") : strcpy(format, "%s/md5/%s.md5");
    sprintf(path, format, jobID, fileName);

    checkPath(rootPath, path, true);

    return std::string(path);
}

void Util::init() {

    DIR *unixDir = opendir(UNIXSOCKET_PATH);
    if (!unixDir) {
        printf("Can not open unix socket path!!!");
        return;
    }

    dirent *entry;
    char path[300];

    while ((entry = readdir(unixDir)) != nullptr) {

        if (strncmp(entry->d_name, UNIXSOCKET_FILE_PREFIX, strlen(UNIXSOCKET_FILE_PREFIX)) == 0) {
            sprintf(path, "%s%s", UNIXSOCKET_PATH, entry->d_name);
            unlink(path);
        }
    }

    closedir(unixDir);
}

std::string Util::extractFile(const char *path) {

    std::string file = path;
    size_t pos = file.find_last_of('/');
    file = file.substr(pos + 1);
    pos = file.find_first_of('.');
    return file.substr(0, pos);
}

void Util::replaceStr(std::string &main, const std::string &search,
                      const std::string &replace) {
    size_t pos = 0;
    while ((pos = main.find(search, pos)) != std::string::npos) {
        main.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

const char* Util::trimEndLine(char *str) {
    while(*str == '\r' || *str == '\n') str++;
    char *end = &str[strlen(str) - 1];
    while(*end == '\r' || *end == '\n') *end-- = '\0';
    return str;
}

bool Util::extractZipFile(const char *file, const char *dest) {

    mz_zip_archive zip_archive;
    mz_zip_archive_file_stat file_stat;
    char absPath[PATH_MAX];

    memset(&zip_archive, 0, sizeof(zip_archive));

    mz_bool status = mz_zip_reader_init_file(&zip_archive, file, 0);
    if (!status) {
        printf("mz_zip_reader_init_file() failed!\n");
        return false;
    }

    for (int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {

        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            printf("mz_zip_reader_file_stat() failed!\n");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }

        sprintf(absPath, "%s/%s", dest, file_stat.m_filename);

        mkPath(absPath);

        status = mz_zip_reader_extract_to_file(&zip_archive, i, absPath, 0);
        if (!status) {
            printf("mz_zip_reader_extract_file_to_cfile() failed!\n");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

    }

    mz_zip_reader_end(&zip_archive);

    return true;
}
