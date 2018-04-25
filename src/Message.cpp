//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"

Message::Message(COMPONENT host)
		: header() {

    setHost(host);

    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(STREAM_NONE);

}

Message::Message(COMPONENT owner, MSG_TYPE type)
		: header() {

    setHost(owner);

    getHeader()->setType(type);
    getHeader()->setOwner(owner);
    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(STREAM_NONE);
}

bool Message::readFile(int desc, FileItem *content, const char* jobDir, long *state, Block *header) {

    if (header->getType() != BLOCK_FILE_BINARY && header->getType() != BLOCK_FILE_INFO) {
        LOG_E("%s : readFile can not read other blocks", ComponentTypes::getName(getHost()));
        return false;
    }

	long id;
    if (!readNumber(desc, &id)) {
        LOG_E("%s : readFile can not read id data", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!readNumber(desc, state)) {
        LOG_E("%s : readFile can not read id data", ComponentTypes::getName(getHost()));
        return false;
    }

	char fileName[PATH_MAX];
	if (!readString(desc, fileName, header->getSize(0))) {
		LOG_E("%s : readFile can not read path data", ComponentTypes::getName(getHost()));
		return false;
	}

    content->set(getHost(), jobDir, fileName, (int)id);

    if (header->getType() != BLOCK_FILE_BINARY || (*state & FILEINFO_OUTPUT)) {
        return true;
    }

    Md5 calcMD5;
	if (!readBinary(desc, Util::getAbsRefPath(getHost(), content->getJobDir(),
                                              fileName).c_str(), &calcMD5, header->getSize(1))) {
		LOG_E("%s : readFile can not read Binary data", ComponentTypes::getName(getHost()));
		return false;
	}

    content->setMD5(&calcMD5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, Block* header) {

    if (header->getType() != BLOCK_FILE_MD5) {
        LOG_E("%s : readFileMD5 can not read other blocks", ComponentTypes::getName(getHost()));
        return false;
    }

	if (!readMD5(desc, content)) {
		LOG_E("%s : readFileMD5 Can not read md5", ComponentTypes::getName(getHost()));
		return false;
	}

	return true;
}

bool Message::readJobInfo(int desc, char *jobDir, Block *header) {

    if (header->getType() != BLOCK_JOB_INFO) {
        LOG_E("%s : readJobInfo can not read other blocks", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!readString(desc, jobDir, header->getSize(0))) {
        LOG_E("%s : readJobInfo can not read job dir", ComponentTypes::getName(getHost()));
        return false;
    }

    return true;
}

bool Message::readExecutionInfo(int desc, char *execution, Block *header) {

    if (header->getType() != BLOCK_EXECUTION_INFO) {
        LOG_E("%s : readExecutionInfo can not read other blocks", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!readString(desc, execution, header->getSize(0))) {
        LOG_E("%s : readExecutionInfo can not read execution info", ComponentTypes::getName(getHost()));
        return false;
    }

    return true;
}

bool Message::readMessageBlock(int in, Block *header) {

    switch(header->getType()) {

        case BLOCK_FILE_INFO:
        case BLOCK_FILE_BINARY: {

            auto *fileItem = new FileItem(getHost());
            long state;

            if (!readFile(in, fileItem, getData()->getJobDir(), &state, header)) {
                return false;
            }

            LOG_I("%s : New %s with path %s/%s received", ComponentTypes::getName(getHost()),
                  header->getType() == BLOCK_FILE_INFO ? "File Info" : "File Binary",
                  fileItem->getJobDir(), fileItem->getFileName());

            getData()->addFile(FileInfo(fileItem, state));
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

            if (!readJobInfo(in, getData()->getJobDir(), header)) {
                return false;
            }

            LOG_I("%s : New job info %s received", ComponentTypes::getName(getHost()), getData()->getJobDir());

            break;

        case BLOCK_EXECUTION_INFO:

            if (!readExecutionInfo(in, getData()->getExecutor(), header)) {
                return false;
            }

            LOG_I("%s : New execution info %s received", ComponentTypes::getName(getHost()), getData()->getExecutor());

            break;


        default:
            return false;
    }

    return true;
}

bool Message::readFinalize() {

    return true;
}

bool Message::writeJobInfo(int desc, char *jobDir) {

    Block blockHeader(1, BLOCK_JOB_INFO);

    blockHeader.setSize(0, (int) strlen(jobDir));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOG_E("%s : writeJobInfo can not write block header", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!writeString(desc, jobDir)) {
        LOG_E("%s : writeJobInfo can not write job info", ComponentTypes::getName(getHost()));
        return false;
    }

    return true;
}

bool Message::writeExecutionInfo(int desc, char *executor) {

    Block blockHeader(1, BLOCK_EXECUTION_INFO);

    blockHeader.setSize(0, (int) strlen(executor));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOG_E("%s : writeExecutionInfo can not write block header", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!writeString(desc, executor)) {
        LOG_E("%s : writeExecutionInfo can not write execution info", ComponentTypes::getName(getHost()));
        return false;
    }

    return true;
}

bool Message::writeFile(int desc, FileItem *content, long state, bool isBinary) {

    Block blockHeader;
    std::string absPath = "";
    bool isBinaryTransfer = !(state & FILEINFO_OUTPUT) && isBinary;

    if (!isBinaryTransfer) {
        blockHeader.set(1, BLOCK_FILE_INFO);
        blockHeader.setSize(0, (uint32_t)strlen(content->getFileName()));
    } else {
        blockHeader.set(2, BLOCK_FILE_BINARY);
        blockHeader.setSize(0, (uint32_t)strlen(content->getFileName()));
        absPath = Util::getAbsRefPath(content->getHost(), content->getJobDir(), content->getFileName());
        blockHeader.setSize(1, getBinarySize(absPath.c_str()));
    }

	if (!writeBlockHeader(desc, &blockHeader)) {
        LOG_E("%s : writeFile can not write block header", ComponentTypes::getName(getHost()));
		return false;
	}

    if (!writeNumber(desc, content->getID())) {
        LOG_E("%s : writeFile can not write file ID", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!writeNumber(desc, state)) {
        LOG_E("%s : writeFile can not write file state", ComponentTypes::getName(getHost()));
        return false;
    }

	if (!writeString(desc, content->getFileName())) {
        LOG_E("%s : writeFile can not write file name", ComponentTypes::getName(getHost()));
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeBinary(desc, absPath.c_str(), NULL, blockHeader.getSize(1))) {
            LOG_E("%s : writeFile can not write Binary data", ComponentTypes::getName(getHost()));
            return false;
        }
    }

	return true;

}

bool Message::writeFileMD5(int desc, Md5 *md5) {

    Block blockHeader(0, BLOCK_FILE_MD5);

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOG_E("%s : writeFileMD5 can not write block header", ComponentTypes::getName(getHost()));
        return false;
    }

    if (!writeMD5(desc, md5)) {
        LOG_E("%s : writeFileMD5 can not write md5 data", ComponentTypes::getName(getHost()));
        return false;
    }

    return true;
}

bool Message::writeMessageStream(int out) {

    int i = 0;

    switch(getData()->getStreamFlag()) {

        case STREAM_INFO:
        case STREAM_BINARY:

            if (!writeJobInfo(out, getData()->getJobDir())) {
                return false;
            }

            if (!writeExecutionInfo(out, getData()->getExecutor())) {
                return false;
            }

            for (i = 0; i < getData()->getFileCount(); i++) {

                if (!writeFile(out, getData()->getFile(i), getData()->getState(i),
                               getData()->getStreamFlag() == STREAM_BINARY)) {
                    return false;
                }
            }

            LOG_I("%s : %d %s's sent to network", ComponentTypes::getName(getHost()), i,
                  getData()->getStreamFlag() == STREAM_INFO ? "File Info" : "File Binary");
            break;

        case STREAM_MD5:

            if (!writeJobInfo(out, getData()->getJobDir())) {
                return false;
            }

            for (i = 0; i < getData()->getMD5Count(); i++) {

                if (!writeFileMD5(out, getData()->getMD5(i))) {
                    return false;
                }
            }

            LOG_I("%s : %d file md5 content sent to network", ComponentTypes::getName(getHost()), i);
            break;

        case STREAM_JOB:

            if (!writeJobInfo(out, getData()->getJobDir())) {
                return false;
            }

            LOG_I("%s : Job : %s sent to network", ComponentTypes::getName(getHost()), getData()->getJobDir());
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

MessageHeader *Message::getHeader() {
    return &header;
}

bool Message::setHeader(const uint8_t *buffer) {
    return getHeader()->set(buffer);
}

bool Message::extractHeader(uint8_t *buffer) {
    return getHeader()->extract(buffer);
}

int Message::getHeaderSize() {
    return getHeader()->getSize();
}

MessageData *Message::getData() {
    return &data;
}
