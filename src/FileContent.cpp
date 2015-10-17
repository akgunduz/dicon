//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileContent.h"
#include "Rule.h"
#include "Util.h"

FileContent::FileContent() {

	memset(mMD5, 0, MD5_DIGEST_LENGTH);
	setValid(false);
	mFlaggedToSent = true;
	mPath = "";
}

FileContent::FileContent(const std::string &rootpath,
		const std::string &path, const char *smd5) : FileContent() {

	mPath = path;

	std::string abspath = rootpath + path;

	FILE *execfile = fopen(abspath.c_str(), "r");
	if (execfile == nullptr) {
		LOG_T("FileContent %s could not opened", abspath.c_str());
		setValid(false);
		return;
	}

	setValid(true);
    mFlaggedToSent = true;
	LOG_T("FileContent is opened");

	if (smd5 != nullptr && strcmp(smd5, "") != 0) {
		Util::str2hex(mMD5, smd5, (uint32_t)strlen(smd5));
		fclose(execfile);
		return;
	}

	char buf[BUFFER_SIZE];

	//Check for md5 file

//	std::string smdf = abspath.substr(0, abspath.rfind('.')) + ".md5";
	std::string smdf = abspath + ".md5";

	FILE *md5file = fopen(smdf.c_str(), "r");
	if (md5file != nullptr) {
		fgets(buf, MD5_DIGEST_LENGTH * 2 + 1, md5file);
		if (Util::str2hex(mMD5, buf, MD5_DIGEST_LENGTH * 2)) {
			fclose(execfile);
			fclose(md5file);
			return;
		}
        fclose(md5file);
	}

	//No md5 file create one
	MD5_CTX ctx;
	MD5_Init(&ctx);

	while(true) {
		int count = fread(buf, 1, BUFFER_SIZE, execfile);
		if (count != BUFFER_SIZE) {
			MD5_Update(&ctx, buf, (unsigned)count);
			break;
		}
		MD5_Update(&ctx, buf, (unsigned)BUFFER_SIZE);
	}
	fclose(execfile);
	MD5_Final(mMD5, &ctx);

	// and write back

	md5file = fopen(smdf.c_str(), "w");
	fwrite(Util::hex2str(mMD5, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
	fputc('\0', md5file);
	fclose(md5file);

};

CONTENT_TYPES FileContent::getType() {
	return CONTENT_FILE;
}

std::string FileContent::getPath() {
	return mPath;
}

void FileContent::setPath(const std::string &path) {
	mPath = path;
}

uint8_t* FileContent::getMD5() {
	return mMD5;
}

void FileContent::setMD5(uint8_t *md5) {
	memcpy(mMD5, md5, MD5_DIGEST_LENGTH);
}

const bool FileContent::isFlaggedToSent() {
	return mFlaggedToSent;
}

void FileContent::setFlaggedToSent(bool flaggedToSent) {
	mFlaggedToSent = flaggedToSent;
}

void FileContent::set(FileContent *content) {
	mPath = content->getPath();
	memcpy(mMD5, content->getMD5(), MD5_DIGEST_LENGTH);
	setValid(content->isValid());
	mFlaggedToSent = content->isFlaggedToSent();
}