//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "ArchTypes.h"

FileItem::FileItem(COMPONENT host)
        : ContentItem () {

    set(host, "", "", -1, NULL);
}

FileItem::FileItem(FileItem *item)
        : ContentItem () {

    set(item->getHost(), item->getJobDir(), item->getFileName(), item->getID(), item->getMD5());
}

FileItem::FileItem(COMPONENT host, const char *jobDir, const char *fileName, int id, Md5 *md5)
        : ContentItem () {

    set(host, jobDir, fileName, id, md5);
};

void FileItem::set(COMPONENT host, const char *jobDir, const char *fileName, int id, Md5 *md5) {

    this->is_validated = false;

    this->host = host;

    this->id = id;

    strcpy(this->jobDir, jobDir);

    strcpy(this->fileName, fileName);

    if (md5 != NULL) {

        this->md5.set(md5);
    }

}

CONTENT_TYPES FileItem::getType() {

	return CONTENT_FILE;
}

COMPONENT FileItem::getHost() {

    return host;
}

const char *FileItem::getJobDir() {

    return jobDir;
}

const char* FileItem::getFileName() {

	return fileName;
}

Md5* FileItem::getMD5() {

    return &md5;
}

bool FileItem::isValid() {

    return is_validated;
}

void FileItem::setMD5(Md5 *md5) {

    if (md5 == nullptr) {
        this->md5.reset();
        return;
    }

    this->md5.set(md5);
}

int FileItem::getID() {

    return id;
}

bool FileItem::validate() {

    if (is_validated) {
        LOG_T("FileContent %s is already validated", getFileName());
        return true;
    }

    if (strcmp(getJobDir(), "") == 0 || strcmp(getFileName(), "") == 0) {
        LOG_T("FileContent %s could not opened", getFileName());
        return false;
    }

    FILE *file = fopen(Util::getAbsRefPath(getHost(), getJobDir(), getFileName()).c_str(), "r");
    if (file == NULL) {
        LOG_T("FileContent %s could not opened", getFileName());
        return false;
    }

    bool status = getMD5()->get(Util::getAbsMD5Path(getHost(), getJobDir(), getFileName()).c_str());
    if (!status) {

        char buf[BUFFER_SIZE];

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

        getMD5()->set(nullptr, Util::getAbsMD5Path(getHost(), getJobDir(), getFileName()).c_str());
    }

    fclose(file);

    is_validated = true;

    return true;
}

