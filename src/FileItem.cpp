//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "ArchTypes.h"

FileItem::FileItem(Unit host)
        : ContentItem () {

    setMD5();
    setFile(Unit(COMP_MAX), NULL, "", FILE_MAX);
	setFlaggedToSent(true);
    this->host = host;
}


FileItem::FileItem(const char *jobDir, FileItem *item)
        : ContentItem () {

    set(jobDir, item);
}

FileItem::FileItem(Unit host, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5)
        : ContentItem () {

    set(host, jobDir, fileName, fileType, md5);
};

FileItem::FileItem(Unit host, Unit node, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5)
        : ContentItem () {

    set(host, node, jobDir, fileName, fileType, md5);
};

bool FileItem::set(Unit host, Unit node, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5) {

    this->host = host;

    setFile(node, jobDir, fileName, fileType);
    setFlaggedToSent(true);

    FILE *file = fopen(Util::absPath(host, getRefPath().c_str()).c_str(), "r");
    if (file == nullptr) {
        LOG_T("FileContent %s could not opened");
        return false;
    }

    if (md5 != nullptr) {
        this->md5.set(md5, Util::absPath(host, getMD5Path().c_str()).c_str());
        return true;
    }

    bool status = this->md5.get(Util::absPath(host, getMD5Path().c_str()).c_str());
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

        this->md5.set(nullptr, Util::absPath(host, getMD5Path().c_str()).c_str());
    }

    fclose(file);

    return true;
}

bool FileItem::set(Unit host, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5) {

    return set(host, Unit(COMP_MAX), jobDir, fileName, fileType, md5);
}

bool FileItem::set(const char *jobDir, FileItem *item) {

    return set(item->getHost(), jobDir, item->getFileName(),
        item->getFileType(), item->getMD5());
}

CONTENT_TYPES FileItem::getType() {
	return CONTENT_FILE;
}

const char* FileItem::getFileName() {
	return fileName;
}

FILETYPE FileItem::getFileType() {
    return fileType;
}

Md5* FileItem::getMD5() {
    return &md5;
}

std::string FileItem::getRefPath() {

    return getPath(false);
}

std::string FileItem::getMD5Path() {

    return getPath(true);
}

std::string FileItem::getPath(bool type) {

    char format[PATH_MAX];
    char path[PATH_MAX];

    !type ? strcpy(format, "%s/%s") : strcpy(format, "%s/md5/%s.md5");
    sprintf(path, format, jobDir, fileName);

    Util::checkPath(Unit::getRootPath(host.getType()), path, true);

    return std::string(path);
}

bool FileItem::isFlaggedToSent() {
	return flaggedToSent;
}

void FileItem::setFlaggedToSent(bool flaggedToSent) {
	this->flaggedToSent = flaggedToSent;
}


void FileItem::setFile(Unit node, const char* jobDir, const char *fileName, FILETYPE fileType) {

    this->jobDir = jobDir;
    this->node = node;
    this->fileType = fileType;
    strcpy(this->fileName, fileName);

//    if (fileType == FILE_MAX) {
//        return;
//    }
//
//    if (host.getType() == COMP_COLLECTOR && fileType == FILE_COMMON) {
//        sprintf(refPath, "%s/common/%s", jobDir, fileName);
//        sprintf(md5Path, "%s/md5/common/%s.md5", jobDir, fileName);
//    } else if(host.getType() == COMP_COLLECTOR && fileType == FILE_ARCH) {
//        sprintf(refPath, "%s/arch/%s/%s", jobDir, ArchTypes::getDir(node.getArch()), fileName);
//        sprintf(md5Path, "%s/md5/arch/%s/%s.md5", jobDir, ArchTypes::getDir(node.getArch()), fileName);
//    } else {
//        sprintf(refPath, "%s/%s", jobDir, fileName);
//        sprintf(md5Path, "%s/md5/%s.md5",  jobDir, fileName);
//    }
//
//    Util::checkPath(Unit::getRootPath(host.getType()), refPath, true);
//    Util::checkPath(Unit::getRootPath(host.getType()), md5Path, true);
}

bool FileItem::isValid() {
    return !this->md5.empty();
}

void FileItem::setMD5(Md5 *md5) {

    if (md5 == nullptr) {
        this->md5.reset();
        return;
    }

    this->md5.set(md5);
}

Unit FileItem::getHost() {
    return host;
}

const char *FileItem::getJobDir() {
    return jobDir;
}
