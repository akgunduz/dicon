//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"

Message::Message(ComponentObject host)
		: header(), BaseMessage(host) {

    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(FLAG_NONE);

}

Message::Message(ComponentObject host, MSG_TYPE type)
		: header(), BaseMessage(host) {

    getHeader()->setType(type);
    getHeader()->setOwner(host);
    getHeader()->setPriority(MESSAGE_DEFAULT_PRIORITY);

    getData()->setStreamFlag(FLAG_NONE);
}

bool Message::readJobID(int desc, Uuid *jobID, Block *header) {

    if (header->getType() != FLAG_JOBID) {
        LOGS_E(getHost(), "readJobID can not read other blocks");
        return false;
    }

    if (!readArray(desc, jobID->get(), jobID->getSize())) {
        LOGS_E(getHost(), "readJobID Can not read array");
        return false;
    }

    return true;
}

bool Message::readJobDir(int desc, char *jobDir, Block *header) {

    if (header->getType() != FLAG_JOBDIR) {
        LOGS_E(getHost(), "readJobDir can not read other blocks");
        return false;
    }

    if (!readString(desc, jobDir, header->getSize(0))) {
        LOGS_E(getHost(), "readJobDir can not read job directory");
        return false;
    }

    return true;
}

bool Message::readProcessID(int desc, long *processID, Block *header) {

    if (header->getType() != FLAG_PROCESSID) {
        LOGS_E(getHost(), "readProcessID can not read other blocks");
        return false;
    }

    if (!readNumber(desc, processID)) {
        LOGS_E(getHost(), "readProcessID can not read process ID");
        return false;
    }

    return true;
}

bool Message::readProcessCommand(int desc, char *command, Block *header) {

    if (header->getType() != FLAG_PROCESSCMD) {
        LOGS_E(getHost(), "readProcessCommand can not read other blocks");
        return false;
    }

    if (!readString(desc, command, header->getSize(0))) {
        LOGS_E(getHost(), "readProcessCommand can not read process command");
        return false;
    }

    return true;
}

bool Message::readFile(int desc, FileItem *content, const char* jobDir, long *state, Block *header) {

    if (header->getType() != FLAG_FILEINFO && header->getType() != FLAG_FILEBINARY) {
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

    content->set(jobDir, fileName, (int)id);

    if (header->getType() != FLAG_FILEBINARY || (*state == FILEINFO_OUTPUT)) {
        return true;
    }

    Md5 calcMD5;
    const char *binPath = Util::getAbsRefPath(getHost().getRootPath(), content->getJobDir(), fileName).c_str();
	if (!readBinary(desc, binPath, &calcMD5, header->getSize(1))) {
		LOGS_E(getHost(), "readFile can not write Binary data to the file system at path : %s, errno : %d", binPath, errno);
		return false;
	}

    content->setMD5(&calcMD5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, Block* header) {

    if (header->getType() != FLAG_FILEMD5) {
        LOGS_E(getHost(), "readFileMD5 can not read other blocks");
        return false;
    }

	if (!readArray(desc, content->data, MD5_DIGEST_LENGTH)) {
		LOGS_E(getHost(), "readFileMD5 Can not read md5");
		return false;
	}

	return true;
}

bool Message::readMessageBlock(int in, Block *header) {

    switch(header->getType()) {

        case FLAG_JOBID: {

            Uuid id;
            if (!readJobID(in, &id, header)) {
                return false;
            }

            LOGS_T(getHost(), "New Job[%s] ID is received",
                   id.getStr().c_str());

            getData()->setJobID(id);
        }
            break;

        case FLAG_JOBDIR:

            if (!readJobDir(in, getData()->getJobDir(), header)) {
                return false;
            }

            LOGS_T(getHost(), "New Job at %s is received",
                   getData()->getJobDir());

            break;

        case FLAG_PROCESSID: {

            long processID;
            if (!readProcessID(in, &processID, header)) {
                return false;
            }

            LOGS_T(getHost(), "New Process ID : %d received", processID);

            getData()->setProcessID(processID);
        }
            break;

        case FLAG_PROCESSCMD:

            if (!readProcessCommand(in, getData()->getProcessCommand(), header)) {
                return false;
            }

            LOGS_T(getHost(), "New Process command :  %s received",
                   getData()->getProcessCommand());

            break;

        case FLAG_FILEINFO:
        case FLAG_FILEBINARY: {

            auto *fileItem = new FileItem(getHost());
            long state;

            if (!readFile(in, fileItem, getData()->getJobDir(), &state, header)) {
                return false;
            }

            LOGS_T(getHost(), "New %s with path %s/%s received",
                  header->getType() == FLAG_FILEBINARY ? "File Binary" : "File Info",
                  fileItem->getJobDir(), fileItem->getFileName());

            getData()->addFile(FileInfo(fileItem, state == FILEINFO_OUTPUT), header->getType() == FLAG_FILEBINARY);
        }
            break;

        case FLAG_FILEMD5: {

            Md5 md5;
            if (!readFileMD5(in, &md5, header)) {
                return false;
            }

            LOGS_T(getHost(), "New md5 info %s received", md5.getStr().c_str());

            getData()->addMD5(md5);
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

bool Message::writeJobID(int desc, Uuid jobID) {

    Block blockHeader(0, FLAG_JOBID);

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeJobID can not write block header");
        return false;
    }

    if (!writeArray(desc, jobID.get(), jobID.getSize())) {
        LOGS_E(getHost(), "writeJobID can not write job ID");
        return false;
    }

    return true;
}

bool Message::writeJobDir(int desc, char *jobDir) {

    Block blockHeader(1, FLAG_JOBDIR);
    blockHeader.setSize(0, (int) strlen(jobDir));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeJobDir can not write block header");
        return false;
    }

    if (!writeString(desc, jobDir)) {
        LOGS_E(getHost(), "writeJobDir can not write job directory");
        return false;
    }

    return true;
}

bool Message::writeProcessID(int desc, long processID) {

    Block blockHeader(0, FLAG_PROCESSID);

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeProcessID can not write block header");
        return false;
    }

    if (!writeNumber(desc, processID)) {
        LOGS_E(getHost(), "writeProcessID can not write process ID");
        return false;
    }

    return true;
}

bool Message::writeProcessCommand(int desc, char *command) {

    Block blockHeader(1, FLAG_PROCESSCMD);

    blockHeader.setSize(0, (int) strlen(command));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeProcessCommand can not write block header");
        return false;
    }

    if (!writeString(desc, command)) {
        LOGS_E(getHost(), "writeProcessCommand can not write process command");
        return false;
    }

    return true;
}

bool Message::writeFile(int desc, FileItem *content, bool isOutput, bool isBinary) {

    Block blockHeader;
    int binarySize = 0;
    std::string absPath;
    bool isBinaryTransfer = !isOutput && isBinary;

    if (!isBinaryTransfer) {
        blockHeader.set(1, FLAG_FILEINFO);
        blockHeader.setSize(0, (uint32_t)strlen(content->getFileName()));
    } else {
        if (strcmp(content->getJobDir(), "") == 0) {
            LOGS_E(getHost(), "writeFile can not write file binary without job directory info, set job directory first!!!");
            return false;
        }
        blockHeader.set(2, FLAG_FILEBINARY);
        blockHeader.setSize(0, (uint32_t)strlen(content->getFileName()));
        absPath = Util::getAbsRefPath(content->getHost().getRootPath(), content->getJobDir(), content->getFileName());
        binarySize = getBinarySize(absPath.c_str());
        if (binarySize == 0) {
            LOGS_E(getHost(), "writeFile can not read Binary data from the file system at path : %s, errno : %d", absPath.c_str(), errno);
            return false;
        }
        blockHeader.setSize(1, binarySize);
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

        if (!writeBinary(desc, absPath.c_str(), NULL, (size_t)binarySize)) {
            LOGS_E(getHost(), "writeFile can not read Binary data from the file system at path : %s, errno : %d", absPath.c_str(), errno);
            return false;
        }
    }

	return true;

}

bool Message::writeFileMD5(int desc, Md5 *md5) {

    Block blockHeader(0, FLAG_FILEMD5);

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeFileMD5 can not write block header");
        return false;
    }

    if (!writeArray(desc, md5->data, MD5_DIGEST_LENGTH)) {
        LOGS_E(getHost(), "writeFileMD5 can not write md5 data");
        return false;
    }

    return true;
}

bool Message::writeMessageStream(int out) {

    int i = 0;

    unsigned long flag = getData()->getStreamFlag();

    if (flag & FLAG_JOBID) {

        if (!writeJobID(out, getData()->getJobID())) {
            return false;
        }

        LOGS_T(getHost(), "Job[%s] ID sent to network",
               getData()->getJobID().getStr().c_str());
    }

    if (flag & FLAG_JOBDIR) {

        if (!writeJobDir(out, getData()->getJobDir())) {
            return false;
        }

        LOGS_T(getHost(), "Job Directory : %s sent to network",
               getData()->getJobDir());
    }

    if (flag & FLAG_PROCESSID) {

        if (!writeProcessID(out, getData()->getProcessID())) {
            return false;
        }

        LOGS_T(getHost(), "Process[%ld] ID sent to network",
               getData()->getProcessID());
    }

    if (flag & FLAG_PROCESSCMD) {

        if (!writeProcessCommand(out, getData()->getProcessCommand())) {
            return false;
        }

        LOGS_T(getHost(), "Process Command : %s sent to network",
               getData()->getProcessCommand());
    }

    if (flag & FLAG_FILE) {

        for (i = 0; i < getData()->getFileCount(); i++) {

            if (!writeFile(out, getData()->getFile(i),
                           getData()->isOutput(i),
                           (flag & FLAG_FILEBINARY) > 0)) {
                return false;
            }
        }

        LOGS_T(getHost(), "%d %s's sent to network", i,
               flag & FLAG_FILEBINARY ? "File Binary" : "File Info");
    }

    if (flag & FLAG_FILEMD5) {

        for (i = 0; i < getData()->getMD5Count(); i++) {

            if (!writeFileMD5(out, getData()->getMD5(i))) {
                return false;
            }
        }

        LOGS_T(getHost(), "%d file md5 content sent to network", i);
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
