//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "ArchTypes.h"

FileItem::FileItem(const ComponentObject& host)
        : ContentItem (), host(host) {

    set("", "", 0, -1, nullptr);
}

FileItem::FileItem(FileItem *item)
        : ContentItem (), host(item->getHost()) {

    set(item->getJobDir(), item->getFileName(), item->getSize(), item->getID(), item->getMD5());
}

FileItem::FileItem(const ComponentObject& host, const char *jobDir, const char *fileName, long size, int id, Md5 *md5)
        : ContentItem (), host(host) {

    set(jobDir, fileName, size, id, md5);
};

void FileItem::set(const char *_jobDir, const char *_fileName, const long _size, const int _id, const Md5 *_md5) {

    this->is_exist = false;

    this->id = _id;

    this->size = _size;

    strcpy(this->jobDir, _jobDir);

    strcpy(this->fileName, _fileName);

    if (_md5 != nullptr) {

        this->md5.set(_md5);
    }
}

CONTENT_TYPES FileItem::getType() const {

	return CONTENT_FILE;
}

ComponentObject FileItem::getHost() const {

    return host;
}

const char *FileItem::getJobDir() const {

    return jobDir;
}

const char* FileItem::getFileName() const {

	return fileName;
}

Md5* FileItem::getMD5() {

    return &md5;
}

bool FileItem::isValid() {

    return validate();
}

void FileItem::setMD5(Md5 *_md5) {

    if (_md5 == nullptr) {
        this->md5.reset();
        return;
    }

    this->md5.set(_md5);
}

int FileItem::getID() const {

    return id;
}

long FileItem::getSize() const {

    return size;
}

bool FileItem::validate() {

    if (is_exist) {
        LOGS_T(getHost(), "FileContent %s is already validated", getFileName());
        return true;
    }

    if (strcmp(getJobDir(), "") == 0 || strcmp(getFileName(), "") == 0) {
        LOGS_T(getHost(), "FileContent %s could not opened", getFileName());
        return false;
    }

    FILE *file = fopen(Util::getAbsRefPath(getHost().getRootPath(), getJobDir(), getFileName()).c_str(), "r");
    if (file == nullptr) {
        LOGS_T(getHost(), "FileContent %s could not opened", getFileName());
        return false;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    if (size == 0) {
        LOGS_T(getHost(), "FileContent %s is empty", getFileName());
        return false;
    }

    bool status = getMD5()->get(Util::getAbsMD5Path(getHost().getRootPath(), getJobDir(), getFileName()).c_str());
    if (!status) {

        char buf[BUFFER_SIZE];
        fseek(file, 0, SEEK_SET);
        //No md5 file create one
        MD5_CTX ctx;
        MD5_Init(&ctx);

        while(true) {
            int count = (int)fread(buf, 1, BUFFER_SIZE, file);
            if (count != BUFFER_SIZE) {
                MD5_Update(&ctx, buf, (unsigned)count);
                break;
            }
            MD5_Update(&ctx, buf, (unsigned)BUFFER_SIZE);
        }

        MD5_Final(getMD5()->data, &ctx);

        getMD5()->set(nullptr, Util::getAbsMD5Path(getHost().getRootPath(), getJobDir(), getFileName()).c_str());
    }

    fclose(file);

    is_exist = true;

    return true;
}


