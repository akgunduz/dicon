//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"

Message::Message(ComponentObject host)
		: header(), BaseMessage(host) {

    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(STREAM_NONE);

}

Message::Message(ComponentObject host, MSG_TYPE type)
		: header(), BaseMessage(host) {

    getHeader()->setType(type);
    getHeader()->setOwner(host);
    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(STREAM_NONE);
}

bool Message::readFile(int desc, FileItem *content, const char* jobDir, long *state, Block *header) {

    if (header->getType() != BLOCK_FILE_BINARY && header->getType() != BLOCK_FILE_INFO) {
        LOGS_E(getHost(), "readFile can not read other blocks");
        return false;
    }

	long id;
    if (!readNumber(desc, &id)) {
        LOGS_E(getHost(), "readFile can not read id data");
        return false;
    }

    if (!readNumber(desc, state)) {
        LOGS_E(getHost(), "readFile can not read id data");
        return false;
    }

	char fileName[PATH_MAX];
	if (!readString(desc, fileName, header->getSize(0))) {
		LOGS_E(getHost(), "readFile can not read path data");
		return false;
	}

    content->set(getHost().getType(), jobDir, fileName, (int)id);

    if (header->getType() != BLOCK_FILE_BINARY || (*state == FILEINFO_OUTPUT)) {
        return true;
    }

    Md5 calcMD5;
	if (!readBinary(desc, Util::getAbsRefPath(getHost(), content->getJobDir(),
                                              fileName).c_str(), &calcMD5, header->getSize(1))) {
		LOGS_E(getHost(), "readFile can not read Binary data");
		return false;
	}

    content->setMD5(&calcMD5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, Block* header) {

    if (header->getType() != BLOCK_FILE_MD5) {
        LOGS_E(getHost(), "readFileMD5 can not read other blocks");
        return false;
    }

	if (!readMD5(desc, content)) {
		LOGS_E(getHost(), "readFileMD5 Can not read md5");
		return false;
	}

	return true;
}

bool Message::readJobInfo(int desc, char *jobDir, Block *header) {

    if (header->getType() != BLOCK_JOB_INFO) {
        LOGS_E(getHost(), "readJobInfo can not read other blocks");
        return false;
    }

    if (!readString(desc, jobDir, header->getSize(0))) {
        LOGS_E(getHost(), "readJobInfo can not read job dir");
        return false;
    }

    return true;
}

bool Message::readExecutionInfo(int desc, long *executionID, char *execution, Block *header) {

    if (header->getType() != BLOCK_EXECUTION_INFO) {
        LOGS_E(getHost(), "readExecutionInfo can not read other blocks");
        return false;
    }

    if (!readNumber(desc, executionID)) {
        LOGS_E(getHost(), "readExecutionInfo can not read execution id");
        return false;
    }

    if (!readString(desc, execution, header->getSize(0))) {
        LOGS_E(getHost(), "readExecutionInfo can not read execution info");
        return false;
    }

    return true;
}

bool Message::readMessageBlock(int in, Block *header) {

    switch(header->getType()) {

        case BLOCK_FILE_INFO:
        case BLOCK_FILE_BINARY: {

            auto *fileItem = new FileItem(getHost().getType());
            long state;

            if (!readFile(in, fileItem, getData()->getJobDir(), &state, header)) {
                return false;
            }

            LOGS_I(getHost(), "New %s with path %s/%s received",
                  header->getType() == BLOCK_FILE_INFO ? "File Info" : "File Binary",
                  fileItem->getJobDir(), fileItem->getFileName());

            getData()->addFile(FileInfo(fileItem, state == FILEINFO_OUTPUT));
        }
            break;

        case BLOCK_FILE_MD5: {

            Md5 md5;
            if (!readFileMD5(in, &md5, header)) {
                return false;
            }

            LOGS_I(getHost(), "New md5 info %s received", md5.getStr().c_str());

            getData()->addMD5(md5);
        }
            break;

        case BLOCK_JOB_INFO:

            if (!readJobInfo(in, getData()->getJobDir(), header)) {
                return false;
            }

            LOGS_I(getHost(), "New job info %s received", getData()->getJobDir());

            break;

        case BLOCK_EXECUTION_INFO:

            long executionID;
            if (!readExecutionInfo(in, &executionID, getData()->getExecutor(), header)) {
                return false;
            }

            LOGS_I(getHost(), "New execution info %s received", getData()->getExecutor());

            getData()->setExecutorID(executionID);

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
        LOGS_E(getHost(), "writeJobInfo can not write block header");
        return false;
    }

    if (!writeString(desc, jobDir)) {
        LOGS_E(getHost(), "writeJobInfo can not write job info");
        return false;
    }

    return true;
}

bool Message::writeExecutionInfo(int desc, long executorID, char *executor) {

    Block blockHeader(1, BLOCK_EXECUTION_INFO);

    blockHeader.setSize(0, (int) strlen(executor));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeExecutionInfo can not write block header");
        return false;
    }

    if (!writeNumber(desc, executorID)) {
        LOGS_E(getHost(), "writeExecutionInfo can not write execution ID");
        return false;
    }

    if (!writeString(desc, executor)) {
        LOGS_E(getHost(), "writeExecutionInfo can not write execution info");
        return false;
    }

    return true;
}

bool Message::writeFile(int desc, FileItem *content, bool isOutput, bool isBinary) {

    Block blockHeader;
    std::string absPath = "";
    bool isBinaryTransfer = !isOutput && isBinary;

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
        LOGS_E(getHost(), "writeFile can not write block header");
		return false;
	}

    if (!writeNumber(desc, content->getID())) {
        LOGS_E(getHost(), "writeFile can not write file ID");
        return false;
    }

    if (!writeNumber(desc, (long)isOutput)) {
        LOGS_E(getHost(), "writeFile can not write file state");
        return false;
    }

	if (!writeString(desc, content->getFileName())) {
        LOGS_E(getHost(), "writeFile can not write file name");
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeBinary(desc, absPath.c_str(), NULL, blockHeader.getSize(1))) {
            LOGS_E(getHost(), "writeFile can not write Binary data");
            return false;
        }
    }

	return true;

}

bool Message::writeFileMD5(int desc, Md5 *md5) {

    Block blockHeader(0, BLOCK_FILE_MD5);

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeFileMD5 can not write block header");
        return false;
    }

    if (!writeMD5(desc, md5)) {
        LOGS_E(getHost(), "writeFileMD5 can not write md5 data");
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

            if (!writeExecutionInfo(out, getData()->getExecutorID(), getData()->getExecutor())) {
                return false;
            }

            for (i = 0; i < getData()->getFileCount(); i++) {

                if (!writeFile(out, getData()->getFile(i), getData()->isOutput(i),
                               getData()->getStreamFlag() == STREAM_BINARY)) {
                    return false;
                }
            }

            LOGS_I(getHost(), "%d %s's sent to network", i,
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

            LOGS_I(getHost(), "%d file md5 content sent to network", i);
            break;

        case STREAM_JOB:

            if (!writeJobInfo(out, getData()->getJobDir())) {
                return false;
            }

            LOGS_I(getHost(), "Job : %s sent to network", getData()->getJobDir());
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
