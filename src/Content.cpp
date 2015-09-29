//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Content.h"
#include "Tools.h"
#include "Rule.h"

Content::Content() {

	mIsValid = false;

}

bool Content::isValid() {

	return mIsValid;

}

void Content::setValid(bool valid) {

	mIsValid = valid;

}

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
	LOG_T("FileContent is opened");

	if (smd5 != nullptr && strcmp(smd5, "") != 0) {
		Tools::str2hex(mMD5, smd5, (uint32_t)strlen(smd5));
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
		if (Tools::str2hex(mMD5, buf, MD5_DIGEST_LENGTH * 2)) {
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
	fwrite(Tools::hex2str(mMD5, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
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

ParamContent::ParamContent(const char *param) {

	setValid(true);

	if (strncmp(param, "l:", 2) == 0) {
		mType = PARAM_LONG;
		mParams.latom = atol(param + 2);
	} else if (strncmp(param, "d:", 2) == 0) {
		mType = PARAM_DOUBLE;
		mParams.datom = atof(param + 2);
	} else {
		mType = PARAM_STRING;
		strcpy(mParams.sPtr, param + 2);
	}
};

CONTENT_TYPES ParamContent::getType() {
	return CONTENT_PARAM;
}

PARAM_TYPES ParamContent::getParamType() {
	return mType;
}

Params ParamContent::getParam() {
	return mParams;
}

void ParamContent::setParam(Params *param) {
	memcpy(&mParams, param, sizeof(Params));
}

ExecutorContent::ExecutorContent(const char *line) {

	setValid(true);

	exec = line;
}

CONTENT_TYPES ExecutorContent::getType() {
	return CONTENT_EXECUTOR;
}

std::string ExecutorContent::getExec() {
	return exec;
}

void ExecutorContent::setExec(const std::string &exec) {
	this->exec = exec;
}


std::string ExecutorContent::getParsed(void *pRule) {

//	Rule *refRule = (Rule *)pRule;
	std::string parsed = "";
	bool cmdMode = false;

	int cmdIndex = 0;
	RULE_TYPES cmdType = RULE_FILES;

	for (uint32_t i = 0; i < exec.size(); i++) {
		switch(exec[i]) {
			case '$':
				if (!cmdMode) {
					cmdMode = true;
					cmdIndex = 0;
					cmdType = RULE_MAX;
					break;
				}
				cmdMode = false;
				//no break
			case 'F':
			case 'f':
				if (cmdMode) {
					cmdType = RULE_FILES;
					break;
				}
				//no break
			case 'P':
			case 'p':
				if (cmdMode) {
					cmdType = RULE_PARAMETERS;
					break;
				}
				//no break
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (cmdMode) {
					cmdIndex = cmdIndex * 10 + (exec[i] - '0');
					break;
				}
				//no break
			case ' ':
				if (cmdMode) {
					cmdMode = false;
					parseCommand(parsed, pRule, cmdType, cmdIndex);
				}
				//no break
			default:
				parsed += exec[i];
				break;

		}

	}

	if (cmdMode) {
		parseCommand(parsed, pRule, cmdType, cmdIndex);
	}

	return parsed;
}

bool ExecutorContent::parseCommand(std::string &parsed, void *pRule, int cmdType, int cmdIndex) {

	Rule *refRule = (Rule *)pRule;

	if (cmdType == RULE_FILES) {
		FileContent *content = (FileContent *) refRule->getContent(RULE_FILES, cmdIndex);
		if (content != nullptr) {
			parsed += refRule->getRootPath() + content->getPath();
		}

	} else if (cmdType == RULE_PARAMETERS) {
		ParamContent *content = (ParamContent *) refRule->getContent(RULE_PARAMETERS, cmdIndex);
		if (content != nullptr) {
			switch(content->getParamType()) {
				case PARAM_LONG:
                    parsed += content->getParam().latom;
                    break;
				case PARAM_DOUBLE:
					parsed += content->getParam().datom;
					break;
				case PARAM_STRING:
					parsed += content->getParam().sPtr;
					break;
			}
		}
	}

	return true;
}


