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
	strcpy(absPath, "");
}

FileContent::FileContent(const char *absPath,
		const char* path, const char* md5Path, FILETYPE type) : FileContent() {

	FILE *file = fopen(absPath, "r");
	if (file == nullptr) {
		LOG_T("FileContent %s could not opened", absPath);
		setValid(false);
		return;
	}

    setPath(path);
    setAbsPath(absPath);
    setValid(true);
    setFlaggedToSent(true);
    setFileType(type);

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

	md5file = fopen(md5Path, "w");
	fwrite(Util::hex2str(md5, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
//	fputc('\0', md5file);
	fclose(md5file);
};

CONTENT_TYPES FileContent::getType() {
	return CONTENT_FILE;
}

const char* FileContent::getPath() {
	return path;
}

void FileContent::setPath(const char *path) {
    strcpy(this->path, path);
}

const char* FileContent::getAbsPath() {
    return absPath;
}

void FileContent::setAbsPath(const char *path) {
    strcpy(this->absPath, path);
}

uint8_t* FileContent::getMD5() {
	return md5;
}

void FileContent::setMD5(uint8_t *md5) {
	memcpy(this->md5, md5, MD5_DIGEST_LENGTH);
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

void FileContent::setFileType(FILETYPE fileType) {
    this->fileType = fileType;
}

void FileContent::set(FileContent *content) {

    setPath(content->getPath());
    setMD5(content->getMD5());
	setValid(content->isValid());
	setFlaggedToSent(content->isFlaggedToSent());
    setFileType(content->getFileType());
}