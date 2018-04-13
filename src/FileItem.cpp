//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "ArchTypes.h"

FileItem::FileItem(COMPONENT host)
        : ContentItem () {

    set(host, "", "", NULL, true);
}

FileItem::FileItem(FileItem *item)
        : ContentItem () {

    set(item->getHost(), item->getJobDir(), item->getFileName(), item->getMD5(), !item->isValid());
}

FileItem::FileItem(COMPONENT host, const char *jobDir, const char *fileName, Md5 *md5, bool info_status)
        : ContentItem () {

    set(host, jobDir, fileName, md5, info_status);
};

bool FileItem::set(COMPONENT host, const char *jobDir, const char *fileName, Md5 *md5, bool info_status) {

    this->host = host;

    strcpy(this->jobDir, jobDir);

    strcpy(this->fileName, fileName);

    this->info_only = false;

    if (info_status) {

        this->info_only = true;

        if (md5 != NULL) {

            this->md5.set(md5);
        }

        return true;
    }

    if (strcmp(jobDir, "") == 0 || strcmp(fileName, "") == 0) {
        LOG_T("FileContent %s could not opened");
        return false;
    }

    FILE *file = fopen(Util::getAbsRefPath(host, jobDir, fileName).c_str(), "r");
    if (file == NULL) {
        LOG_T("FileContent %s could not opened");
        return false;
    }

    if (md5 != NULL) {
        this->md5.set(md5, Util::getAbsMD5Path(host, jobDir, fileName).c_str());
        return true;
    }

    bool status = this->md5.get(Util::getAbsMD5Path(host, jobDir, fileName).c_str());
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

        MD5_Final(this->md5.data, &ctx);

        this->md5.set(nullptr, Util::getAbsMD5Path(host, jobDir, fileName).c_str());
    }

    fclose(file);

    return true;
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

    return !info_only;
}

void FileItem::setMD5(Md5 *md5) {

    if (md5 == nullptr) {
        this->md5.reset();
        return;
    }

    this->md5.set(md5);
}
