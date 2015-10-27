//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "Rule.h"
#include "Util.h"
#include "ArchTypes.h"

FileItem::FileItem(const char* rootPath) {

	memset(this->md5, 0, MD5_DIGEST_LENGTH);
    strcpy(this->rootPath, rootPath);
    strcpy(this->fileName, "");
    this->fileType = FILE_MAX;
	setFlaggedToSent(true);
}


FileItem::FileItem(FileItem *item)
        : FileItem(item->getRootPath(), item->getFileName(),
                   item->getFileType(), item->getMD5()) {

}

FileItem::FileItem(const char *rootPath, const char *fileName,
                   FILETYPE fileType, uint8_t *md5) : FileItem(rootPath) {

    if (md5 != nullptr) {
        memcpy(this->md5, md5, MD5_DIGEST_LENGTH);

    } else {
        memset(this->md5, 0, MD5_DIGEST_LENGTH);
        char buf[BUFFER_SIZE];

        FILE *md5file = fopen(getMD5Path().c_str(), "r");
        if (md5file != nullptr) {
            fgets(buf, MD5_DIGEST_LENGTH * 2 + 1, md5file);
            if (Util::str2hex(this->md5, buf, MD5_DIGEST_LENGTH * 2)) {
                fclose(md5file);
                return;
            }
            fclose(md5file);
        }
    }

    setFile(rootPath, fileName, fileType);
    setFlaggedToSent(true);
/*
	FILE *file = fopen(getAbsPath(node), "r");
	if (file == nullptr) {
		LOG_T("FileContent %s could not opened");
		setValid(false);
		return;
	}

    setValid(true);
*/



/*
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
	MD5_Final(md5, &ctx);

	// and write back
    setMD5(md5);
*/
};

CONTENT_TYPES FileItem::getType() {
	return CONTENT_FILE;
}

const char*FileItem::getFileName() {
	return fileName;
}

std::string FileItem::getAbsPath() {

    std::string absPath = std::string(rootPath) + "/" + fileName;

    if (!absPath.empty() && access(absPath.c_str(), F_OK ) == -1) {
        Util::mkPath(absPath.c_str());
    }

    return absPath;

}

std::string FileItem::getAbsPath(Unit host, Unit node) {

    std::string absPath = "";

    if (host.getType() == HOST_COLLECTOR && fileType == FILE_COMMON) {
        absPath = std::string(rootPath) + "/common/" + fileName;

    } else if(host.getType() == HOST_COLLECTOR && fileType == FILE_ARCH) {
        absPath = std::string(rootPath) + "/arch/" + (char*)ArchTypes::getDir((ARCH_IDS)node.getID()) + "/" + fileName;

    } else {
        absPath = std::string(rootPath) + "/" + fileName;

    }

    if (!absPath.empty() && access(absPath.c_str(), F_OK ) == -1) {
        Util::mkPath(absPath.c_str());
    }

    return absPath;

}

std::string FileItem::getMD5Path() {

    std::string md5Path = std::string(rootPath) + "/md5/" + fileName + ".md5";

    if (access(md5Path.c_str(), F_OK ) == -1) {
        Util::mkPath(md5Path.c_str());
    }

	return md5Path;
}

uint8_t*FileItem::getMD5() {
	return md5;
}

void FileItem::setMD5(uint8_t *md5) {

	memcpy(this->md5, md5, MD5_DIGEST_LENGTH);

    FILE *md5file = fopen(getMD5Path().c_str(), "w");
	fwrite(Util::hex2str(md5, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
	fclose(md5file);
}

bool FileItem::isFlaggedToSent() {
	return flaggedToSent;
}

void FileItem::setFlaggedToSent(bool flaggedToSent) {
	this->flaggedToSent = flaggedToSent;
}

FILETYPE FileItem::getFileType() {
    return fileType;
}

void FileItem::setFile(const char* rootPath,
                       const char *fileName, FILETYPE fileType) {

    strcpy(this->rootPath, rootPath);
    strcpy(this->fileName, fileName);
    this->fileType = fileType;
}

const char *FileItem::getRootPath() {
    return rootPath;
}

void FileItem::setRootPath(const char *rootPath) {
    strcpy(this->rootPath, rootPath);
}