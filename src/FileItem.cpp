//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "ArchTypes.h"

FileItem::FileItem(Unit host, const char* rootPath)
        : ContentItem () {

    setMD5();
    setFile(Unit(HOST_MAX), rootPath, "", "", FILE_MAX);
	setFlaggedToSent(true);
    this->host = host;
}


FileItem::FileItem(FileItem *item)
        : ContentItem () {

    set(item);
}

FileItem::FileItem(Unit host, const char *rootPath, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5)
        : ContentItem () {

    set(host, rootPath, jobDir, fileName, fileType, md5);
};

FileItem::FileItem(Unit host, Unit node, const char *rootPath, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5)
        : ContentItem () {

    set(host, node, rootPath, jobDir, fileName, fileType, md5);
};

bool FileItem::set(Unit host, Unit node, const char *rootPath, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5) {

    this->host = host;

    setFile(node, rootPath, jobDir, fileName, fileType);
    setFlaggedToSent(true);
    setValid(false);

    if (md5 != nullptr) {
        this->md5.set(md5, getMD5Path());
        setValid(true);
        return true;
    }

    bool status = this->md5.get(getMD5Path());
    if (!status) {
        FILE *file = fopen(getAbsPath(), "r");
        if (file == nullptr) {
            LOG_T("FileContent %s could not opened");
            setValid(false);
            return false;
        }

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
        fclose(file);
        MD5_Final(this->md5.data, &ctx);

        // and write back
        FILE *md5file = fopen(getMD5Path(), "w");
        fwrite(Util::hex2str(this->md5.data, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
        fclose(md5file);
    }

    setValid(true);
    return true;
}

bool FileItem::set(Unit host, const char *rootPath, const char *jobDir, const char *fileName,
                   FILETYPE fileType, Md5 *md5) {

    set(host, Unit(HOST_MAX), rootPath, jobDir, fileName, fileType, md5);
}

void FileItem::set(FileItem *item) {

    set(item->getHost(), item->getRootPath(), item->getJobDir(), item->getFileName(),
        item->getFileType(), item->getMD5());
}

const char* FileItem::getRootPath() {
    return rootPath;
}

void FileItem::setRootPath(const char *rootPath) {
    strcpy(this->rootPath, rootPath);
}

CONTENT_TYPES FileItem::getType() {
	return CONTENT_FILE;
}

const char* FileItem::getJobDir() {
    return jobDir;
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

const char* FileItem::getAbsPath() {
    return absPath;
}

const char* FileItem::getMD5Path() {
	return md5Path;
}

bool FileItem::isFlaggedToSent() {
	return flaggedToSent;
}

void FileItem::setFlaggedToSent(bool flaggedToSent) {
	this->flaggedToSent = flaggedToSent;
}


void FileItem::setFile(Unit node, const char* rootPath, const char *jobDir,
                       const char *fileName, FILETYPE fileType) {

    strcpy(this->rootPath, rootPath);
    strcpy(this->fileName, fileName);
    strcpy(this->jobDir, jobDir);
    this->fileType = fileType;

    if (host.getType() == HOST_COLLECTOR && fileType == FILE_COMMON) {
        sprintf(absPath, "%s/%s/common/%s", rootPath, jobDir, fileName);
        sprintf(md5Path, "%s/%s/md5/common/%s.md5", rootPath, jobDir, fileName);
        //absPath = std::string(rootPath) + "/" + jobDir + "/common/" + fileName;
    } else if(host.getType() == HOST_COLLECTOR && fileType == FILE_ARCH) {
        sprintf(absPath, "%s/%s/arch/%s/%s", rootPath, jobDir, ArchTypes::getDir((ARCH_IDS)node.getID()), fileName);
        sprintf(md5Path, "%s/%s/md5/arch/%s/%s.md5", rootPath, jobDir, ArchTypes::getDir((ARCH_IDS)node.getID()), fileName);
        //absPath = std::string(rootPath) + "/" + jobDir + "/arch/" + (char*)ArchTypes::getDir((ARCH_IDS)node.getID()) + "/" + fileName;
    } else {
        sprintf(absPath, "%s/%s/%s", rootPath, jobDir, fileName);
        sprintf(md5Path, "%s/%s/md5/%s.md5", rootPath, jobDir, fileName);
        //absPath = std::string(rootPath) + "/" + jobDir + "/" + fileName;
    }

    if (access(absPath, F_OK ) == -1) {
        Util::mkPath(absPath);
    }

    if (access(md5Path, F_OK ) == -1) {
        Util::mkPath(md5Path);
    }
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
