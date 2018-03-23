//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"

Message::Message(Unit host)
		: BaseMessage(sizeof(MessageHeader)), header() {

    setHost(host);
    setStreamFlag(STREAM_NONE);
    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);
}

Message::Message(Unit owner, int type)
		: BaseMessage(sizeof(MessageHeader)), header() {

    getHeader()->setType(type);
    getHeader()->setOwner(owner);

    setHost(owner);
    setStreamFlag(STREAM_NONE);
    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);
}

void Message::setStreamFlag(int flag) {
    streamFlag = flag;
}

Unit Message::getHost() {
    return this->host;
}

void Message::setHost(Unit host) {
    this->host = host;
}

void Message::setJob(int streamFlag, FileList *fileList) {

    setStreamFlag(streamFlag);

    this->fileList = fileList;
}

bool Message::readMD5(int desc, Md5* md5) {

	if (!readBlock(desc, md5->data, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not read MD5 from stream");
		return false;
	}

	return true;
}

bool Message::readFileBinary(int desc, FileItem *content, Block *header) {

	if (header->getType() != BLOCK_FILE_BINARY) {
		LOG_E("readFileBinary can not read other blocks");
		return false;
	}

    char jobDir[MAX_JOB_DIR_LENGTH];
    if (!readString(desc, jobDir, header->getSize(0))) {
        LOG_E("readFileBinary can not read path data");
        return false;
    }

	char fileName[MAX_FILE_NAME_LENGTH];
	if (!readString(desc, fileName, header->getSize(1))) {
		LOG_E("readFileBinary can not read path data");
		return false;
	}

    long fileType;
    if (!readNumber(desc, &fileType)) {
        LOG_E("readFileBinary can not read filetype data");
        return false;
    }

    content->setFile(getHeader()->getOwner(), jobDir, fileName, (FILETYPE)fileType);

    std::string absPath = Util::absPath(getHost(), content->getRefPath());

    Md5 calcMD5;
	if (!readBinary(desc, absPath.c_str(), &calcMD5, header->getSize(2))) {
		LOG_E("readFileBinary can not read Binary data");
		return false;
	}

    Md5 md5;
    if (!readMD5(desc, &md5)) {
        LOG_E("readFileBinary can not read MD5 data");
        return false;
    }

	if (!md5.equal(&calcMD5)) {
		LOG_E("readFileBinary mismatch in md5 data");
		return false;
	}

    content->setMD5(&md5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, Block* header) {

	if (header->getSize(0) != MD5_DIGEST_LENGTH) {
		LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
		return false;
	}

	if (!readMD5(desc, content)) {
		LOG_E("Can not read md5");
		return false;
	}

	return true;
}

bool Message::readJobInfo(int desc, char *jobDir, Block *header) {

    if (!readString(desc, jobDir, header->getSize(0))) {
        LOG_E("readFileBinary can not read path data");
        return false;
    }

    return true;
}


bool Message::readMessageBlock(int in, Block *header) {

    switch(header->getType()) {

        case BLOCK_FILE_BINARY: {

            FileItem *fileItem = new FileItem(getHost());

            if (!readFileBinary(in, fileItem, header)) {
                return false;
            }

            LOG_E("New binary with path %s/%s", fileItem->getJobDir(), fileItem->getFileName());

            delete fileItem;
        }
            break;

        case BLOCK_FILE_MD5: {

            Md5 md5;
            if (!readFileMD5(in, &md5, header)) {
                return false;
            }

            md5List.push_back(md5);
        }
            break;

        case BLOCK_JOB_INFO:

            if (!readJobInfo(in, jobDir, header)) {
                return false;
            }

            break;

        default:
            return false;
    }

    return true;
}

bool Message::readFinalize() {

    return true;
}

bool Message::writeMD5(int desc, Md5* md5) {

	if (!writeBlock(desc, md5->data, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not write md5 to stream");
		return false;
	}

	return true;
}

bool Message::writeFileBinary(int desc, FileItem *content) {

	Block blockHeader(3, BLOCK_FILE_BINARY);

    std::string absPath = Util::absPath(getHost(), content->getRefPath());

    blockHeader.setSize(0, (uint32_t)strlen(content->getJobDir()));
    blockHeader.setSize(1, (uint32_t)strlen(content->getFileName()));
    blockHeader.setSize(2, getBinarySize(absPath.c_str()));

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

    if (!writeString(desc, content->getJobDir())) {
        return false;
    }

	if (!writeString(desc, content->getFileName())) {
        return false;
    }

    if (!writeNumber(desc, content->getFileType())) {
        return false;
    }

	Md5 calcMD5;
	if (!writeBinary(desc, absPath.c_str(), &calcMD5, blockHeader.getSize(2))) {
		LOG_E("writeFileBinary can not write Binary data");
		return false;
	}

    content->setMD5(&calcMD5);

	if (!writeMD5(desc, &calcMD5)) {
        LOG_E("writeFileBinary can not write md5 data");
		return false;
	}

	return true;

}

bool Message::writeFileMD5(int desc, FileItem *content) {

    Block blockHeader(1, BLOCK_FILE_MD5);

    blockHeader.setSize(0, MD5_DIGEST_LENGTH);

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    return writeMD5(desc, content->getMD5());
}

bool Message::writeJobInfo(int desc, const char *jobDir) {

    Block blockHeader(1, BLOCK_JOB_INFO);

    blockHeader.setSize(0, (int) strlen(jobDir));

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    return writeString(desc, jobDir);
}

bool Message::writeMessageStream(int out) {

    int contentCount;

    switch(streamFlag) {

        case STREAM_JOB:

            if (!writeJobInfo(out, fileList->getDir())) {
                return false;
            }

            contentCount = fileList->process(this, &Message::writeFileBinary, FILE_LIST_ALL);

            LOG_E("%d job content sent to network", contentCount);
            break;

        case STREAM_BINARY:

            contentCount = fileList->process(this, &Message::writeFileBinary, FILE_LIST_TRUE);

            LOG_E("%d binary content sent to network", contentCount);

            break;

        case STREAM_MD5ONLY:

            contentCount = fileList->process(this, &Message::writeFileMD5, FILE_LIST_TRUE);

            LOG_E("%d md5 content sent to network", contentCount);
            break;

        case STREAM_NONE:
        default :
            break;
    }

    return true;

}

bool Message::writeFinalize() {
    return true;
}

const char *Message::getJobDir() {
    return jobDir;
}

MessageHeader *Message::getHeader() {
    return &header;
}

bool Message::setHeader(const uint8_t *buffer) {
    return getHeader()->set(buffer);
}

bool Message::extractHeader(uint8_t *buffer) {
    return getHeader()->extract(buffer);
}
