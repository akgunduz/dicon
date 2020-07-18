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

bool Message::readComponentList(int desc, std::vector<ComponentObject> &componentList, Block* block) {

    if (block->getType() != BLOCK_COMPONENT_LIST) {
        LOGS_E(getHost(), "readComponentList can not read other blocks");
        return false;
    }

    std::vector<long> list;

    if (!readNumberList(desc, list, block->getSize(0))) {
        LOGS_E(getHost(), "readNumberList can not read number");
        return false;
    }

    for (int i = 0; i < list.size(); i=i+2) {

        componentList.emplace_back(ComponentObject((COMPONENT)block->getSize(1), list[i], list[i+1]));
    }

    return true;
}

bool Message::readFile(int desc, FileItem *content, const char* jobName, long *state, Block *block) {

    if (block->getType() != BLOCK_FILE_BINARY && block->getType() != BLOCK_FILE_INFO) {
        LOGS_E(getHost(), "readFile can not read other blocks");
        return false;
    }

	long id;
    if (!readNumber(desc, &id)) {
        LOGS_E(getHost(), "readFile can not read id data");
        return false;
    }

    if (!readNumber(desc, state)) {
        LOGS_E(getHost(), "readFile can not read state data");
        return false;
    }

	char fileName[PATH_MAX];
	if (!readString(desc, fileName, block->getSize(0))) {
		LOGS_E(getHost(), "readFile can not read path data");
		return false;
	}

    content->set(jobName, fileName, block->getSize(1), (int)id);

    if (block->getType() != BLOCK_FILE_BINARY || (*state == FILEINFO_OUTPUT)) {
        return true;
    }

    Md5 calcMD5;
	if (!readBinary(desc, Util::getAbsRefPath(getHost().getRootPath(), jobName, fileName).c_str(),
	            &calcMD5, content->getSize())) {
		LOGS_E(getHost(), "readFile can not read Binary data");
		return false;
	}

    content->setMD5(&calcMD5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, Block* block) {

    if (block->getType() != BLOCK_FILE_MD5) {
        LOGS_E(getHost(), "readFileMD5 can not read other blocks");
        return false;
    }

	if (!readMD5(desc, content)) {
		LOGS_E(getHost(), "readFileMD5 Can not read md5");
		return false;
	}

	return true;
}

bool Message::readJobInfo(int desc, char *jobName, Block *block) {

    if (block->getType() != BLOCK_JOB_INFO) {
        LOGS_E(getHost(), "readJobInfo can not read other blocks");
        return false;
    }

    if (!readString(desc, jobName, block->getSize(0))) {
        LOGS_E(getHost(), "readJobInfo can not read job dir");
        return false;
    }

    return true;
}

bool Message::readProcessInfo(int desc, int *processID, char *process, Block *block) {

    if (block->getType() != BLOCK_PROCESS_INFO) {
        LOGS_E(getHost(), "readProcessInfo can not read other blocks");
        return false;
    }

    if (!readNumber(desc, (long *)processID)) {
        LOGS_E(getHost(), "readProcessInfo can not read process id");
        return false;
    }

    if (!readString(desc, process, block->getSize(0))) {
        LOGS_E(getHost(), "readProcessInfo can not read process info");
        return false;
    }

    return true;
}

bool Message::readMessageBlock(int in, Block *block) {

    switch(block->getType()) {

        case BLOCK_FILE_INFO:
        case BLOCK_FILE_BINARY: {

            auto *fileItem = new FileItem(getHost());
            long state;

            if (!readFile(in, fileItem, getData()->getJobName(), &state, block)) {
                return false;
            }

            LOGS_T(getHost(), "New %s with path %s/%s received",
                   block->getType() == BLOCK_FILE_INFO ? "File Info" : "File Binary",
                   fileItem->getJobName(), fileItem->getFileName());

            getData()->addFile(FileInfo(fileItem, state == FILEINFO_OUTPUT));
        }
            break;

        case BLOCK_FILE_MD5: {

            Md5 md5;
            if (!readFileMD5(in, &md5, block)) {
                return false;
            }

            LOGS_T(getHost(), "New md5 info %s received", md5.getStr().c_str());

            getData()->addMD5(md5);
        }
            break;

        case BLOCK_JOB_INFO:

            if (!readJobInfo(in, getData()->getJobName(), block)) {
                return false;
            }

            LOGS_T(getHost(), "New job info %s received", getData()->getJobName());

            break;

        case BLOCK_PROCESS_INFO:

            int processID;
            if (!readProcessInfo(in, &processID, getData()->getProcess(), block)) {
                return false;
            }

            LOGS_T(getHost(), "New process info %s received", getData()->getProcess());

            getData()->setProcessID(processID);

            break;

        case BLOCK_COMPONENT_LIST:{

            std::vector<ComponentObject> &list = getData()->getComponentList();

            if (!readComponentList(in, list, block)) {
                return false;
            }

            LOGS_T(getHost(), "New component list is received");
            break;
        }

        default:
            return false;
    }

    return true;
}

bool Message::readFinalize() {

    return true;
}

bool Message::writeComponentList(int desc, std::vector<ComponentObject>& componentList) {

    std::vector<long> list;

    for (auto &component : componentList) {
        list.emplace_back(component.getID());
        list.emplace_back(component.getAddress());
    }

    Block blockHeader(2, BLOCK_COMPONENT_LIST);

    blockHeader.setSize(0, (int) list.size());
    blockHeader.setSize(1, (int) componentList[0].getType());

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeComponentList can not write block header");
        return false;
    }

    if (!writeNumberList(desc, list)) {
        LOGS_E(getHost(), "writeNumberList can not write component list");
        return false;
    }

    return true;
}

bool Message::writeJobInfo(int desc, char *jobName) {

    Block blockHeader(1, BLOCK_JOB_INFO);

    blockHeader.setSize(0, (int) strlen(jobName));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeJobInfo can not write block header");
        return false;
    }

    if (!writeString(desc, jobName)) {
        LOGS_E(getHost(), "writeJobInfo can not write job info");
        return false;
    }

    return true;
}

bool Message::writeProcessInfo(int desc, int processID, char *process) {

    Block blockHeader(1, BLOCK_PROCESS_INFO);

    blockHeader.setSize(0, (int) strlen(process));

    if (!writeBlockHeader(desc, &blockHeader)) {
        LOGS_E(getHost(), "writeProcessInfo can not write block header");
        return false;
    }

    if (!writeNumber(desc, processID)) {
        LOGS_E(getHost(), "writeProcessInfo can not write process ID");
        return false;
    }

    if (!writeString(desc, process)) {
        LOGS_E(getHost(), "writeProcessInfo can not write process info");
        return false;
    }

    return true;
}

bool Message::writeFile(int desc, FileItem *content, bool isOutput, bool isBinary) {

    Block blockHeader;
    std::string absPath;
    bool isBinaryTransfer = !isOutput && isBinary;

    if (!isBinaryTransfer) {
        blockHeader.set(2, BLOCK_FILE_INFO);
        blockHeader.setSize(0, (uint32_t)strlen(content->getFileName()));
        blockHeader.setSize(1, content->getSize());
    } else {
        blockHeader.set(2, BLOCK_FILE_BINARY);
        blockHeader.setSize(0, (uint32_t)strlen(content->getFileName()));
        absPath = Util::getAbsRefPath(content->getHost().getRootPath(), content->getJobName(), content->getFileName());
        blockHeader.setSize(1, content->getSize());
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

        if (!writeBinary(desc, absPath.c_str(), nullptr, content->getSize())) {
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

            if (!writeJobInfo(out, getData()->getJobName())) {
                return false;
            }

            if (!writeProcessInfo(out, getData()->getProcessID(), getData()->getProcess())) {
                return false;
            }

            for (i = 0; i < getData()->getFileCount(); i++) {

                if (!writeFile(out, getData()->getFile(i), getData()->isOutput(i),
                               getData()->getStreamFlag() == STREAM_BINARY)) {
                    return false;
                }
            }

            LOGS_T(getHost(), "%d %s's sent to network", i,
                  getData()->getStreamFlag() == STREAM_INFO ? "File Info" : "File Binary");
            break;

        case STREAM_MD5:

            if (!writeJobInfo(out, getData()->getJobName())) {
                return false;
            }

            for (i = 0; i < getData()->getMD5Count(); i++) {

                if (!writeFileMD5(out, getData()->getMD5(i))) {
                    return false;
                }
            }

            LOGS_T(getHost(), "%d file md5 content sent to network", i);
            break;

        case STREAM_JOB:

            if (!writeJobInfo(out, getData()->getJobName())) {
                return false;
            }

            LOGS_T(getHost(), "Job : %s sent to network", getData()->getJobName());
            break;

        case STREAM_COMPONENT: {

            if (!writeComponentList(out, getData()->getComponentList())) {
                return false;
            }

            LOGS_T(getHost(), "Job : %s sent to network", getData()->getJobName());
            break;
        }

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

bool Message::deSerializeHeader(const uint8_t *buffer) {
    return getHeader()->deSerialize(buffer);
}

bool Message::serializeHeader(uint8_t *buffer) {
    return getHeader()->serialize(buffer);
}

int Message::getHeaderSize() {
    return getHeader()->getSize();
}

MessageData *Message::getData() {
    return &data;
}
