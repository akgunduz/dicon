//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileContent.h"
#include "Rule.h"
#include "Util.h"

FileContent::FileContent() {

	memset(md5, 0, MD5_DIGEST_LENGTH);
	setValid(false);
	flaggedToSent = true;
	strcpy(path, "");
	strcpy(this->absPath, "");
	strcpy(this->md5Path, "");
}

FileContent::FileContent(Unit host, Unit node, const char* rootPath,
                         const char *path, FILETYPE fileType) : FileContent() {

    setFile(host, node, rootPath, path, fileType);

	FILE *file = fopen(absPath, "r");
	if (file == nullptr) {
		LOG_T("FileContent %s could not opened", absPath);
		setValid(false);
		return;
	}

    setValid(true);
    setFlaggedToSent(true);

	char buf[BUFFER_SIZE];

	FILE *md5file = fopen(md5Path, "r");
	if (md5file != nullptr) {
		fgets(buf, MD5_DIGEST_LENGTH * 2 + 1, md5file);
		if (Util::str2hex(md5, buf, MD5_DIGEST_LENGTH * 2)) {
			fclose(file);
			fclose(md5file);
			return;
		}
        fclose(md5file);
	}

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
};

CONTENT_TYPES FileContent::getType() {
	return CONTENT_FILE;
}

const char* FileContent::getPath() {
	return path;
}

const char* FileContent::getAbsPath() {
    return absPath;
}

const char* FileContent::getMD5Path() {
	return md5Path;
}

uint8_t* FileContent::getMD5() {
	return md5;
}

void FileContent::setMD5(uint8_t *md5) {

	memcpy(this->md5, md5, MD5_DIGEST_LENGTH);

    FILE *md5file = fopen(md5Path, "w");
	fwrite(Util::hex2str(md5, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
	fclose(md5file);
}

bool FileContent::isFlaggedToSent() {
	return flaggedToSent;
}

void FileContent::setFlaggedToSent(bool flaggedToSent) {
	this->flaggedToSent = flaggedToSent;
}

FILETYPE FileContent::getFileType() {
    return fileType;
}

void FileContent::setFile(Unit host, Unit node, const char* rootPath,
                          const char *path, FILETYPE fileType) {

    strcpy(this->path, path);
    this->fileType = fileType;

	switch(host.getType()) {

        case HOST_DISTRIBUTOR:
            sprintf(absPath, "%s/%s", rootPath, path);
            break;

        case HOST_COLLECTOR:
            switch(fileType) {
                case FILE_RULE:
                //    sprintf(absPath, "%s/Job_%s/%s", rootPath, jobID, path);
                    break;
                case FILE_COMMON:
               //     sprintf(absPath, "%s/Job_%s/common/%s", rootPath, jobID, path);
                    break;
                case FILE_ARCH:
                //    sprintf(absPath, "%s/Job_%s/arch/%s/%s", rootPath, jobID, (char*)sArchs[node.getID()], path);
                    break;
            }
            break;

        case HOST_NODE:
       //     sprintf(absPath, "%s/Job_%s/%s", rootPath, jobID, path);
            break;

        default:
            return;
	}

    if (access(absPath, F_OK ) == -1) {
        Util::mkPath(absPath);
    }

    //  char *ptr = absPath + strlen(rootPath);

//    sprintf(md5Path, "%s/md5/Job_%s/%s.md5", rootPath, jobID, path);

    if (access(md5Path, F_OK ) == -1) {
        Util::mkPath(md5Path);
    }
}