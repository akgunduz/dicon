//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "Util.h"

Message::Message(Unit host, const char* rootPath)
		: BaseMessage(host, rootPath) {

	rule = nullptr;
}

Message::Message(Unit owner, int type, const char* rootPath)
		: BaseMessage(owner, type, rootPath) {

	rule = nullptr;
}

void Message::setRule(int streamFlag, Rule *rule) {

    setStreamFlag(streamFlag);

    this->rule = rule;
}

bool Message::readMD5(int desc, uint8_t *md5) {

	if (!readBlock(desc, md5, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not read MD5 from stream");
		return false;
	}

	return true;
}

bool Message::readFileBinary(int desc, FileContent *content, struct BlockHeader *header) {

	if (header->blockType != BLOCK_FILE_BINARY) {
		LOG_E("readFileBinary can not read other blocks");
		return false;
	}

	char path[PATH_MAX];
	if (!readString(desc, path, header->sizes[0])) {
		LOG_E("readFileBinary can not read path data");
		return false;
	}
	content->setPath(path);

	if (header->sizes[1] != MD5_DIGEST_LENGTH) {
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

    char absPath[PATH_MAX];
    char md5Path[PATH_MAX];

    Util::getAbsolutePath(getHost(), getOwner(), FILE_RULE, getRootPath(), path, absPath, md5Path);

	if (!readBinary(desc, absPath, calcmd5, md5Path, header->sizes[2])) {
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

	if (header->sizes[0] != MD5_DIGEST_LENGTH) {
		LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
		return false;
	}

	if (!readMD5(desc, content->md5)) {
		LOG_E("Can not read md5");
		return false;
	}

	return true;
}

bool Message::readMessageBlock(int in, BlockHeader *blockHeader) {

    switch(blockHeader->blockType) {

        case BLOCK_FILE_BINARY: {

            FileContent *fileContent = new FileContent();

            if (!readFileBinary(in, fileContent, blockHeader)) {
                return false;
            }

            if (rule == nullptr) {

                rule = new Rule(Unit(getHost()), Unit(getOwner()), getRootPath());

            }

            if (rule->isValid() && strcmp(fileContent->getPath(), RULE_FILE) == 0) {

                rule->updateFileContent(fileContent);

            }

            delete fileContent;
        }
            break;

        case BLOCK_FILE_MD5: {
            MD5Wrapper md5;
            if (!readFileMD5(in, &md5, blockHeader)) {
                return false;
            }

            md5List.push_back(md5);
        }
            break;

        default:
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

bool Message::writeFileBinary(int desc, FileContent *content) {

	BlockHeader blockHeader = {3, BLOCK_FILE_BINARY};

    blockHeader.sizes[0] = (uint32_t)strlen(content->getPath());
    blockHeader.sizes[1] = MD5_DIGEST_LENGTH;
    blockHeader.sizes[2] = getBinarySize(content->getAbsPath());

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
	if (!writeBinary(desc, content->getAbsPath(), calcmd5)) {
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

	BlockHeader blockHeader = {1, BLOCK_FILE_MD5};

    blockHeader.sizes[0] = MD5_DIGEST_LENGTH;

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	return writeMD5(desc, content->getMD5());
}

bool Message::writeMessageStream(int out, int streamFlag) {

    switch(streamFlag) {

        case STREAM_RULE:
            if (!writeFileBinary(out, rule->getContent())) {
                return false;
            }

            break;

        case STREAM_BINARY:
            for (uint16_t i = 0; i < rule->getContentCount(RULE_FILES); i++) {
                FileContent *content = (FileContent *)rule->getContent(RULE_FILES, i);
                if (content->isFlaggedToSent()) {
                    if (!writeFileBinary(out, content)) {
                        return false;
                    }
                }
            }
            break;

        case STREAM_MD5ONLY:
            for (uint16_t i = 0; i < rule->getContentCount(RULE_FILES); i++) {
                FileContent *content = (FileContent *)rule->getContent(RULE_FILES, i);
                if (content->isFlaggedToSent()) {
                    if (!writeFileMD5(out, content)) {
                        return false;
                    }
                }
            }
            break;

        default :
            break;
    }

    return true;

}
