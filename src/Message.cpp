//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "Tools.h"

Message::Message(const std::string& rootpath) {
	mRule = nullptr;
	mRootPath = rootpath;
	mReceivedBinaryCount = 0;
	setStreamFlag(STREAM_NONE);
	setPriority(DEFAULT_PRIORITY);
}

PRIORITIES Message::getPriority() {
	return (PRIORITIES)mHeader.mPriority;
}

PRIORITIES Message::iteratePriority() {
	if (mHeader.mPriority > PRIORITY_1) {
		mHeader.mPriority--;
	}
	return (PRIORITIES)mHeader.mPriority;
}

void Message::display() {
/*	LOG_S("Header :");
	LOG_S("\t\tType : %d", mHeader.mType);
	LOG_S("\t\tPriority : %d", mHeader.mPriority);
	LOG_S("\t\tPipeAddress : %d", mHeader.mPipeAddress);
	LOG_S("\t\tFlag : %d", mHeader.mFlag);
	LOG_S("\t\tVariant64 : %d", mHeader.mVariant);

	if (mMD5List.size() > 0) {
		LOG_S("MD5 List :");
		for (int i = 0; i < mMD5List.size(); i++) {
			LOG_S("\t\t%s", Tools::getMD5Str(mMD5List[i].md5).c_str());
		}
	}

	if (mParameters.size() > 0) {
		LOG_S("Parameters List :");
		for (int i = 0; i < mParameters.size(); i++) {
			LOG_S("\t\t%s", mParameters[i].c_str());
		}
	}

	if (mParameters.size() > 0) {
		LOG_S("Parameters List :");
		for (int i = 0; i < mParameters.size(); i++) {
			LOG_S("\t\t%s", mParameters[i].c_str());
		}
	}

	if (mExecutors.size() > 0) {
		LOG_S("Executors List :");
		for (int i = 0; i < mExecutors.size(); i++) {
			LOG_S("\t\t%s", mExecutors[i].c_str());
		}
	}*/
}

std::string Message::getPath(uint8_t *md5) {
	for (int i = 0; i < mRule->getContentCount(RULE_FILES); i++) {
		FileContent *content = (FileContent *)mRule->getContent(RULE_FILES, i);
		if (memcmp(content->getMD5(), md5, MD5_DIGEST_LENGTH) == 0) {
			return content->getPath();
		}
	}
	LOG_E("Can not find path info for MD5");
	return "";
}

bool Message::readBlock(int desc, uint8_t *buf, uint32_t size) {

	int offset = 0;
	bool busy = false;

	do {

		long count = read(desc, buf + offset, size);

		if (count == -1) {
			if (errno == EINTR) {
				LOG_W("Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOG_W("Busy state, sleep and retry");
				busy = true;
				usleep(200000);
				continue;
			}

			LOG_E("Can not read data block");
			return false;
		}

		if (count == 0) {
			LOG_E("Empty read operation");
			return false;
		}

		if (count < size) {
			size -= count;
			offset += count;

			if (size == 0) {
				break;
			}

			busy = false;
			continue;
		}

		break;

	} while(1);

	return true;
}

bool Message::transferBinary(int sourcedesc, int targetdesc, uint8_t *md5, uint32_t size) {

	uint8_t buf[BUFFER_SIZE];

	MD5_CTX ctx;
	MD5_Init(&ctx);

	bool error = false;
	uint32_t readsize;

	do {

		if (size > BUFFER_SIZE) {
			readsize = BUFFER_SIZE;
			size -= readsize;
		} else {
			readsize = size;
			size = 0;
		}

		if (!readBlock(sourcedesc, buf, (unsigned)readsize)) {
			LOG_E("Can not read data in transferBinary");
			error = true;
			break;
		}

		if (!writeBlock(targetdesc, buf, (unsigned)readsize)) {
			LOG_E("Can not write data in transferBinary");
			error = true;
			break;
		}

		MD5_Update(&ctx, buf, (unsigned)readsize);

	} while(size);

	MD5_Final(md5, &ctx);

	return !error;

}

bool Message::readSignature(int desc) {

	uint8_t data;

	if (!readBlock(desc, &data, 1)) {
		LOG_E("Can not read correct signature from stream");
		return false;
	}

	if (data != (SIGNATURE & 0xFF)) {
		LOG_E("Can not read correct signature from stream");
		return false;
	}

	if (!readBlock(desc, &data, 1)) {
		LOG_E("Can not read correct signature from stream");
		return false;
	}

	if (data != (SIGNATURE >> 8)) {
		LOG_E("Can not read correct signature from stream");
		return false;
	}

	return true;
}

bool Message::readHeader(int desc, struct MessageHeader *header) {

	if (!readBlock(desc, mTmpBuf, sizeof(struct MessageHeader))) {
		LOG_E("Can not read message header from stream");
		return false;
	}

	uint8_t *p = mTmpBuf;
	header->mOwner = *p; p += 1;
	header->mTarget = *p; p += 1;
	header->mPriority = *p; p += 1;
	header->mType = *p; p += 1;
	header->mPadding = *((uint32_t *) p); p += 4;
	header->mOwnerAddress = *((uint64_t *) p); p += 8;
	header->mVariant = *((uint64_t *) p);

	return true;
}

bool Message::readBlockHeader(int desc, struct BlockHeader *header) {

	if (!readBlock(desc, mTmpBuf, sizeof(struct BlockHeader))) {
		LOG_E("Can not read block header from stream");
		return false;
	}

	unsigned char *p = mTmpBuf;
	header->mBlockType = *((uint16_t *) p); p += 2;
	header->mPadding = *((uint16_t *) p); p += 2;
	header->mSize1 = *((uint32_t *) p); p += 4;
	header->mSize2 = *((uint32_t *) p); p += 4;
	header->mSize3 = *((uint32_t *) p);

	return true;
}

bool Message::readMD5(int desc, uint8_t *md5) {

	if (!readBlock(desc, mTmpBuf, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not read MD5 from stream");
		return false;
	}

	memcpy(md5, mTmpBuf, MD5_DIGEST_LENGTH);
	return true;
}

bool Message::readString(int desc, std::string &object, uint32_t size) {
	object.assign("");
	while(size > sizeof(mTmpBuf) - 1) {
		if (!readBlock(desc, mTmpBuf, sizeof(mTmpBuf) - 1)) {
			LOG_E("Can not read object data from stream");
			return false;
		}
		mTmpBuf[sizeof(mTmpBuf) - 1] = '\0';
		object.append((char *)mTmpBuf);
		size -= sizeof(mTmpBuf) + 1;
	}
	if (!readBlock(desc, mTmpBuf, size)) {
		LOG_E("Can not read object data from stream");
		return false;
	}
	mTmpBuf[size] = '\0';
	object.append((char *)mTmpBuf);
	return true;
}

bool Message::readCharArray(int desc, char *buf, uint32_t size) {
	char *p = buf;
	while(size > sizeof(mTmpBuf) - 1) {
		if (!readBlock(desc, mTmpBuf, sizeof(mTmpBuf) - 1)) {
			LOG_E("Can not read object data from stream");
			return false;
		}
		mTmpBuf[sizeof(mTmpBuf) - 1] = '\0';
		memcpy(p, mTmpBuf, sizeof(mTmpBuf));
		p += sizeof(mTmpBuf);
		size -= sizeof(mTmpBuf) + 1;
	}
	if (!readBlock(desc, mTmpBuf, size)) {
		LOG_E("Can not read object data from stream");
		return false;
	}
	mTmpBuf[size] = '\0';
	memcpy(p, mTmpBuf, size);
	return true;
}

bool Message::readNumber(int desc, uint64_t *number, uint32_t size) {
	//size must be 8;
	if (!readBlock(desc, (uint8_t *)number, 8)) {
		LOG_E("Can not read number from stream");
		return false;
	}
	return true;
}

uint32_t Message::getBinarySize(const std::string &path) {

	FILE *fd = fopen(path.c_str(), "rb");
	if (!fd) {
		LOG_E("File %s could not found", path.c_str());
		return 0;
	}

	fseek(fd, 0, SEEK_END);
	uint32_t filesize = (uint32_t)ftell(fd);

	fclose(fd);

	return filesize;
}

bool Message::readBinary(int desc, const std::string &path, uint8_t *md5, uint32_t size) {

	Tools::mkpath(path.c_str());
	int writedesc = open(path.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (writedesc == -1) {
		LOG_E("File %s could not created or opened", path.c_str());
		return false;
	}

	bool status = transferBinary(desc, writedesc, md5, size);

	close(writedesc);

	if (status) {
//		std::string sMD5 = path.substr(0, path.rfind('.')) + ".md5";
		std::string sMD5 = path + ".md5";
		writedesc = open(sMD5.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 00755);

		if (!writeBlock(writedesc, (const uint8_t*)Tools::getMD5Str(md5).c_str(), MD5_DIGEST_LENGTH * 2)) {
			LOG_E("Can not write md5 to file system in readBinary");
			status = false;
		}

		close(writedesc);
	}

	return status;

}

bool Message::readFileInfo(int desc, FileContent *content, uint32_t size) {
	uint8_t md5[MD5_DIGEST_LENGTH];
	if (!readMD5(desc, md5)) {
		return false;
	}
	content->setMD5(md5);

	std::string path;
	if (!readString(desc, path, size - MD5_DIGEST_LENGTH)) {
		return false;
	}
	content->setPath(path);

	return true;
}

bool Message::readFileBinary(int desc, FileContent *content, struct BlockHeader *header, const std::string &rootpath) {

	if (header->mBlockType != BLOCK_FILE_BINARY) {
		LOG_E("readFileBinary can not read other blocks");
		return false;
	}

	std::string path;
	if (!readString(desc, path, header->mSize1)) {
		LOG_E("readFileBinary can not read path data");
		return false;
	}
	content->setPath(path);

	if (header->mSize2 != MD5_DIGEST_LENGTH) {
		LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
		return false;
	}

	uint8_t md5[MD5_DIGEST_LENGTH];
	if (!readMD5(desc, md5)) {
		LOG_E("readFileBinary can not read MD5 data");
		return false;
	}
	content->setMD5(md5);

	uint8_t calcmd5[MD5_DIGEST_LENGTH];
	if (!readBinary(desc, rootpath + path, calcmd5, header->mSize3)) {
		LOG_E("readFileBinary can not read Binary data");
		return false;
	}

	if (memcmp(md5, calcmd5, MD5_DIGEST_LENGTH) != 0) {
		LOG_E("readFileBinary mismatch in md5 data");
		return false;
	}
	content->setValid(true);

	return true;
}

bool Message::readFileMD5(int desc, MD5Wrapper *content, struct BlockHeader* header) {

	if (header->mSize1 != MD5_DIGEST_LENGTH) {
		LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
		return false;
	}

	if (!readMD5(desc, content->mMD5)) {
		LOG_E("Can not read md5");
		return false;
	}

	return true;
}

bool Message::readParameter(int desc, ParamContent *content, uint32_t size) {

	PARAM_TYPES paramtype;
	if (!readBlock(desc, (uint8_t *)&paramtype, 1)) {
		LOG_E("Can not read parameter type from stream");
		return false;
	}

	Params params;

	switch(paramtype) {
		case PARAM_STRING:
			if (!readCharArray(desc, params.sPtr, size - 1)) {
				return false;
			}
			break;
		case PARAM_LONG:
			if (!readNumber(desc, (uint64_t *)&params.latom, 8)) {
				return false;
			}
			break;
		case PARAM_DOUBLE:
			if (!readNumber(desc, (uint64_t *)&params.datom, 8)) {
				return false;
			}
			break;
		default:
			return false;
	}

	content->setParam(&params);
	return true;
}

bool Message::readExecutor(int desc, ExecutorContent *content, uint32_t size) {

	std::string exec;
	if (!readString(desc, exec, size)) {
		return false;
	}

	content->setExec(exec);
	return true;
}

bool Message::readFromStream(int desc) {

	if (!readSignature(desc)) {
		return false;
	}

	if (!readHeader(desc, &mHeader)) {
		return false;
	}

	uint16_t blockCount = 0;

	struct BlockHeader header;
	do {
		if (!readBlockHeader(desc, &header)) {
			return false;
		}

		blockCount++;

		switch(header.mBlockType) {

			case BLOCK_FILE_BINARY: {
				FileContent *fileContent = new FileContent();
				if (!readFileBinary(desc, fileContent, &header, mRootPath)) {
					return false;
				}

				if (blockCount == 1) {

					mRule = new Rule(mRootPath, RULE_FILE);

				}

				if (mRule->isValid() && fileContent->getPath().compare(RULE_FILE)) {

					mRule->updateFileContent(fileContent);
					mReceivedBinaryCount++;
					//mRule->iterateReceivedBinaryCount();

				}

				delete fileContent;
			}
				break;

			case BLOCK_FILE_MD5: {
				MD5Wrapper md5;
				if (!readFileMD5(desc, &md5, &header)) {
					return false;
				}
				mMD5List.push_back(md5);
			}
				break;

			case BLOCK_END_STREAM:
				return true;
			default:
				return false;
		}

	} while(1);
}

bool Message::writeBlock(int desc, const uint8_t *buf, uint32_t size) {

	int offset = 0;
	bool busy = false;

	do {

		long count = write(desc, buf + offset, size);

		if (count == -1) {
			if (errno == EINTR) {
				LOG_W("Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOG_W("Busy state, sleep and retry");
				busy = true;
				usleep(200000);
				continue;
			}

			LOG_E("Can not write data block");
			return false;
		}

		if (count == 0) {
			LOG_E("Empty write operation");
			return false;
		}

		if (count < size) {
			size -= count;
			offset += count;

			if (size == 0) {
				break;
			}

			busy = false;
			continue;
		}

		break;

	} while(1);

	return true;

}

bool Message::writeSignature(int desc) {
	uint16_t signature = SIGNATURE;
	long count = write(desc, &signature, 2);
	if (count != 2) {
		LOG_E("Can not write signature to stream");
		return false;
	}
	return true;
}

bool Message::writeHeader(int desc, MessageHeader *header) {
	if (!writeBlock(desc, (unsigned char *)header, sizeof(MessageHeader))) {
		LOG_E("Can not write message header to stream");
		return false;
	}
	return true;
}

bool Message::writeBlockHeader(int desc, BlockHeader *header) {
	if (!writeBlock(desc, (unsigned char *)header, sizeof(BlockHeader))) {
		LOG_E("Can not write block header to stream");
		return false;
	}
	return true;
}

bool Message::writeMD5(int desc, uint8_t *md5) {
	if (!writeBlock(desc, md5, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not write md5 to stream");
		return false;
	}
	return true;
}

bool Message::writeString(int desc, const std::string &str) {
	if (!writeBlock(desc, (uint8_t*)str.c_str(), (uint32_t)str.length())) {
		LOG_E("Can not write string to stream");
		return false;
	}
	return true;
}

bool Message::writeCharArray(int desc, const char *str) {
	if (!writeBlock(desc, (uint8_t*)str, (uint32_t)strlen(str))) {
		LOG_E("Can not write char array to stream");
		return false;
	}
	return true;
}

bool Message::writeNumber(int desc, uint64_t number) {
	if (!writeBlock(desc, (uint8_t *)&number, 8)) {
		LOG_E("Can not write char array to stream");
		return false;
	}
	return true;
}

bool Message::writeBinary(int desc, const std::string &path, uint8_t *md5, uint32_t size) {

	int readdesc = open(path.c_str(), O_RDONLY);
	if (readdesc == -1) {
		LOG_E("File %s could not created or opened", path.c_str());
		return false;
	}

	bool status = transferBinary(readdesc, desc, md5, size);

	close(readdesc);

	return status;

}


bool Message::writeFileInfo(int desc, FileContent *content) {

	BlockHeader blockHeader = {BLOCK_FILE_INFO, 0};

	blockHeader.mSize1 = MD5_DIGEST_LENGTH;
	blockHeader.mSize2 = (uint32_t)content->getPath().length();

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	if (!writeMD5(desc, content->getMD5())) {
		return false;
	}

	return writeString(desc, content->getPath());
}

bool Message::writeFileBinary(int desc, FileContent *content, const std::string &rootpath) {

	BlockHeader blockHeader = {BLOCK_FILE_BINARY, 0};

	blockHeader.mSize1 = (uint32_t)content->getPath().length();
	blockHeader.mSize2 = MD5_DIGEST_LENGTH;
	blockHeader.mSize3 = getBinarySize(rootpath + content->getPath());

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	if (!writeString(desc, content->getPath())) {
		return false;
	}

	if (!writeMD5(desc, content->getMD5())) {
		return false;
	}

	uint8_t calcmd5[MD5_DIGEST_LENGTH];
	if (!writeBinary(desc, rootpath + content->getPath(), calcmd5, blockHeader.mSize3)) {
		LOG_E("writeFileBinary can not write Binary data");
		return false;
	}

	if (memcmp(content->getMD5(), calcmd5, MD5_DIGEST_LENGTH) != 0) {
		LOG_E("writeFileBinary mismatch in md5 data");
		return false;
	}

	return true;

}

bool Message::writeFileMD5(int desc, FileContent *content) {

	BlockHeader blockHeader = {BLOCK_FILE_MD5, 0};

	blockHeader.mSize1 = MD5_DIGEST_LENGTH;

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	return writeMD5(desc, content->getMD5());
}

bool Message::writeParameter(int desc, ParamContent *content) {

	BlockHeader blockHeader = {BLOCK_PARAM, 0};

	PARAM_TYPES paramtype = content->getParamType();

	blockHeader.mSize1 = 1;
	blockHeader.mSize2 = paramtype == PARAM_STRING ? (uint32_t)strlen(content->getParam().sPtr) : 8;

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	if (!writeBlock(desc, (uint8_t *)&paramtype, blockHeader.mSize1)) {
		return false;
	}

	switch(paramtype) {
		case PARAM_STRING:
			return writeCharArray(desc, content->getParam().sPtr);
		case PARAM_LONG:
			return writeNumber(desc, (uint64_t)content->getParam().latom);
		case PARAM_DOUBLE:
			return writeNumber(desc, (uint64_t)content->getParam().datom);
		default:
			return false;
	}
}

bool Message::writeExecutor(int desc, ExecutorContent *content) {

	BlockHeader blockHeader = {BLOCK_EXECUTOR, 0};

	blockHeader.mSize1 = (uint32_t)content->getExec().length();

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	return writeString(desc, content->getExec());
}

bool Message::writeEndStream(int desc) {

	BlockHeader blockHeader = {BLOCK_END_STREAM, 0};

	return writeBlockHeader(desc, &blockHeader);
}

bool Message::writeToStream(int desc) {

	if (!writeSignature(desc)) {
		return false;
	}

	if (!writeHeader(desc, &mHeader)) {
		return false;
	}

	switch(mStreamFlag) {

		case STREAM_RULE:
			writeFileBinary(desc, mRule->getRuleFile(), mRootPath);
			break;

		case STREAM_BINARY:
			for (uint16_t i = 0; i < mRule->getContentCount(RULE_FILES); i++) {
				FileContent *content = (FileContent *)mRule->getContent(RULE_FILES, i);
				if (content->isFlaggedToSent()) {
					writeFileBinary(desc, content, mRootPath);
				}
			}
			break;

		case STREAM_MD5ONLY:
			for (uint16_t i = 0; i < mRule->getContentCount(RULE_FILES); i++) {
				FileContent *content = (FileContent *)mRule->getContent(RULE_FILES, i);
				if (content->isFlaggedToSent()) {
					writeFileMD5(desc, content);
				}
			}
			break;

		default :
			break;
	}

	writeEndStream(desc);

	return true;
}

HOST Message::getOwner() {
	return (HOST) mHeader.mOwner;
}

void Message::setOwner(HOST owner) {
	mHeader.mOwner = owner;
}

HOST Message::getTarget() {
	return (HOST) mHeader.mTarget;
}

void Message::setTarget(HOST owner) {
	mHeader.mTarget = owner;
}

uint8_t Message::getType() {
	return mHeader.mType;
}

void Message::setType(uint8_t type) {
	mHeader.mType = type;
}

uint64_t Message::getOwnerAddress() {
	return mHeader.mOwnerAddress;
}

void Message::setOwnerAddress(uint64_t address) {
	mHeader.mOwnerAddress = address;
}

uint64_t Message::getVariant() {
	return mHeader.mVariant;
}

void Message::setVariant(uint64_t variant) {
	mHeader.mVariant = variant;
}

void Message::setPriority(PRIORITIES priority) {
	mHeader.mPriority = priority;
}

void Message::normalizePriority() {
	mHeader.mPriority *= PRIORITY_COEFFICIENT;
}

void Message::setStreamFlag(uint8_t flag) {
	mStreamFlag = flag;
}

void Message::setRule(uint8_t streamFlag, Rule *rule) {
	mStreamFlag = streamFlag;
	mRule = rule;
}


ssize_t Message::getReceivedBinaryCount() {
	return mReceivedBinaryCount;
}
