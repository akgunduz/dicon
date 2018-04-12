//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"

Message::Message(COMPONENT host)
		: BaseMessage(sizeof(MessageHeader)), header() {

    setHost(host);

    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(STREAM_NONE);

}

Message::Message(COMPONENT owner, MSG_TYPE type)
		: BaseMessage(sizeof(MessageHeader)), header() {

    setHost(owner);

    getHeader()->setType(type);
    getHeader()->setOwner(owner);
    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(STREAM_NONE);
}

bool Message::readFileBinary(int desc, FileItem *content, Block *header) {

	if (header->getType() != BLOCK_FILE_BINARY) {
		LOG_E("%s : readFileBinary can not read other blocks", ComponentTypes::getName(getHost()));
		return false;
	}

    char jobDir[MAX_JOB_DIR_LENGTH];
    if (!readString(desc, jobDir, header->getSize(0))) {
        LOG_E("%s : readFileBinary can not read path data", ComponentTypes::getName(getHost()));
        return false;
    }

	char fileName[MAX_FILE_NAME_LENGTH];
	if (!readString(desc, fileName, header->getSize(1))) {
		LOG_E("%s : readFileBinary can not read path data", ComponentTypes::getName(getHost()));
		return false;
	}

    long fileType;
    if (!readNumber(desc, &fileType)) {
        LOG_E("%s : readFileBinary can not read filetype data", ComponentTypes::getName(getHost()));
        return false;
    }

    content->setFile(jobDir, fileName, (FILETYPE)fileType);

    std::string absPath = Util::absPath(getHost(), content->getRefPath().c_str());

    Md5 calcMD5;
	if (!readBinary(desc, absPath.c_str(), &calcMD5, header->getSize(2))) {
		LOG_E("%s : readFileBinary can not read Binary data", ComponentTypes::getName(getHost()));
		return false;
	}

    Md5 md5;
    if (!readMD5(desc, &md5)) {
        LOG_E("%s : readFileBinary can not read MD5 data", ComponentTypes::getName(getHost()));
        return false;
    }

	if (!md5.equal(&calcMD5)) {
		LOG_E("%s : readFileBinary mismatch in md5 data", ComponentTypes::getName(getHost()));
		return false;
	}

    content->setMD5(&md5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, Block* header) {

    if (header->getType() != BLOCK_FILE_MD5) {
        LOG_E("%s : readFileMD5 can not read other blocks", ComponentTypes::getName(getHost()));
        return false;
    }

    if (header->getSize(0) != MD5_DIGEST_LENGTH) {
		LOG_E("%s : Md5 size must be %d long", ComponentTypes::getName(getHost()), MD5_DIGEST_LENGTH);
		return false;
	}

	if (!readMD5(desc, content)) {
		LOG_E("%s : Can not read md5", ComponentTypes::getName(getHost()));
		return false;
	}

	return true;
}

bool Message::readJobInfo(int desc, char *jobDir, char *execution, Block *header) {

    if (header->getType() != BLOCK_JOB_INFO) {
        LOG_E("%s : readJobInfo can not read other blocks", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!readString(desc, jobDir, header->getSize(0))) {
        LOG_E("%s : readFileBinary can not read path data", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!readString(desc, execution, header->getSize(1))) {
        LOG_E("%s : readFileBinary can not read path data", ComponentTypes::getName(getHost()));
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

            LOG_I("%s : New binary with path %s/%s received", ComponentTypes::getName(getHost()),
                  fileItem->getJobDir(), fileItem->getFileName());

            delete fileItem;
        }
            break;

        case BLOCK_FILE_MD5: {

            Md5 md5;
            if (!readFileMD5(in, &md5, header)) {
                return false;
            }

            LOG_I("%s : New md5 info %s received", ComponentTypes::getName(getHost()), md5.getStr().c_str());

            getData()->addMD5(md5);
        }
            break;

        case BLOCK_JOB_INFO:

            if (!readJobInfo(in, getData()->getJobDir(),
                             getData()->getExecutor(), header)) {
                return false;
            }

            LOG_I("%s : New job info %s received", ComponentTypes::getName(getHost()), getData()->getExecutor());

            break;

        default:
            return false;
    }

    return true;
}

bool Message::readFinalize() {

    return true;
}

bool Message::writeFileBinary(int desc, FileItem *content) {

	Block blockHeader(3, BLOCK_FILE_BINARY);

    std::string absPath = Util::absPath(getHost(), content->getRefPath().c_str());

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
		LOG_E("%s : writeFileBinary can not write Binary data", ComponentTypes::getName(getHost()));
		return false;
	}

    content->setMD5(&calcMD5);

	if (!writeMD5(desc, &calcMD5)) {
        LOG_E("%s : writeFileBinary can not write md5 data", ComponentTypes::getName(getHost()));
		return false;
	}

	return true;

}

bool Message::writeFileMD5(int desc, Md5 *md5) {

    Block blockHeader(1, BLOCK_FILE_MD5);

    blockHeader.setSize(0, MD5_DIGEST_LENGTH);

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    return writeMD5(desc, md5);
}

bool Message::writeJobInfo(int desc, char *jobDir, char *executor) {

    Block blockHeader(2, BLOCK_JOB_INFO);

    blockHeader.setSize(0, (int) strlen(jobDir));
    blockHeader.setSize(1, (int) strlen(executor));

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    if (!writeString(desc, jobDir)) {
        return false;
    }

    return writeString(desc, executor);
}

bool Message::writeMessageStream(int out) {

    int i = 0;

    switch(getData()->getStreamFlag()) {

        case STREAM_JOB:

            if (!writeJobInfo(out, getData()->getJobDir(), getData()->getExecutor())) {
                return false;
            }

            for (i = 0; i < getData()->getMD5Count(); i++) {

                writeFileMD5(out, getData()->getMD5(i));
            }

            LOG_I("%s : %d job md5's sent to network", ComponentTypes::getName(getHost()), i);
            break;

        case STREAM_BINARY:

            for (i = 0; i < getData()->getFileCount(); i++) {

                writeFileBinary(out, getData()->getFile(i));
            }

            LOG_I("%s : %d binary content sent to network", ComponentTypes::getName(getHost()), i);

            break;

        case STREAM_MD5ONLY:

            for (i = 0; i < getData()->getMD5Count(); i++) {

                writeFileMD5(out, getData()->getMD5(i));
            }

            LOG_I("%s : %d md5 content sent to network", ComponentTypes::getName(getHost()), i);
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

//const char *Message::getJobDir() {
//    return jobDir;
//}

MessageHeader *Message::getHeader() {
    return &header;
}

bool Message::setHeader(const uint8_t *buffer) {
    return getHeader()->set(buffer);
}

bool Message::extractHeader(uint8_t *buffer) {
    return getHeader()->extract(buffer);
}

MessageData *Message::getData() {
    return &data;
}
