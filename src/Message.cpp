//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "Util.h"

Message::Message(ComponentObject& host)
		: MessageBase(host) {

    header.setPriority(MESSAGE_DEFAULT_PRIORITY);

    data.setStreamFlag(STREAM_NONE);

}

Message::Message(ComponentObject& host, COMPONENT targetType, MSG_TYPE type)
		: MessageBase(host) {

    header.setType(type);
    header.setOwner(host, targetType);
    header.setPriority(MESSAGE_DEFAULT_PRIORITY);

    data.setStreamFlag(STREAM_NONE);
}

bool Message::readComponentList(int desc, std::vector<ComponentObject> &componentList,
        MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_COMPONENT) {
        LOGS_E(getHost(), "readComponentList can not read other blocks");
        return false;
    }

    std::vector<long> list;

    if (!readNumberList(desc, list, block.get(1), crc)) {
        LOGS_E(getHost(), "readNumberList can not read number");
        return false;
    }

    for (int i = 0; i < list.size(); i=i+2) {

        componentList.emplace_back(ComponentObject((COMPONENT)block.get(0), list[i], list[i+1]));
    }

    LOGS_D(getHost(), "Component list is read successfully => Count : %d", componentList.size());
    for (int i = 0; i < componentList.size(); i++) {
        LOGS_D(getHost(), "Component[%d] : %d", i, componentList[0].getID());
    }


    return true;
}

bool Message::readJobName(int desc, char *jobName, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_JOB) {
        LOGS_E(getHost(), "readJobName can not read other blocks");
        return false;
    }

    if (!readString(desc, jobName, block.get(0), crc)) {
        LOGS_E(getHost(), "readJobName can not read job name");
        return false;
    }

    LOGS_D(getHost(), "Job Name is read successfully => Name : %s", jobName);

    return true;
}

bool Message::readProcessID(int desc, long& processID, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_PROCESSID) {
        LOGS_E(getHost(), "readJobName can not read other blocks");
        return false;
    }

    if (!readNumber(desc, processID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }

    LOGS_D(getHost(), "Process ID is read successfully => ID : %ld", processID);

    return true;
}

bool Message::readProcess(int desc, ProcessItem *content, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_PROCESSINFO) {
        LOGS_E(getHost(), "readProcessInfo can not read other blocks");
        return false;
    }

    long id;
    if (!readNumber(desc, id, crc)) {
        LOGS_E(getHost(), "readProcessInfo can not read process id");
        return false;
    }
    content->setID(id);

    long jobID;
    if (!readNumber(desc, jobID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }
    content->setAssignedJob(jobID);

    char process[PATH_MAX];
    if (!readString(desc, process, block.get(0), crc)) {
        LOGS_E(getHost(), "readProcessInfo can not read process info");
        return false;
    }
    content->setParsedProcess(process);

    LOGS_D(getHost(), "Process is read successfully => ID : %ld, jobID : %ld, Process : %s",
            id, jobID, process);

    return true;
}

bool Message::readFile(int desc, ProcessFile& content, MessageBlockHeader &block, uint32_t& crc) {

    if (block.getType() != BLOCK_FILEBINARY && block.getType() != BLOCK_FILEINFO) {
        LOGS_E(getHost(), "readFile can not read other blocks");
        return false;
    }

    long id;
    if (!readNumber(desc, id, crc)) {
        LOGS_E(getHost(), "readFile can not read id data");
        return false;
    }
    content.get()->setID(id);

    long state;
    if (!readNumber(desc, state, crc)) {
        LOGS_E(getHost(), "readFile can not read state data");
        return false;
    }
    content.setOutputState((bool)state);

    long jobID;
    if (!readNumber(desc, jobID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }
    content.get()->setAssignedJob(jobID);

    long processID;
    if (!readNumber(desc, processID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }
    content.setAssignedProcess(processID);

    char fileName[PATH_MAX];
    if (!readString(desc, fileName, block.get(0), crc)) {
        LOGS_E(getHost(), "readFile can not read path data");
        return false;
    }
    content.get()->setName(fileName);

    if (block.getType() == BLOCK_FILEINFO || content.isOutput()) {

        LOGS_D(getHost(), "File Info is read successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
               id, state? "output" : "input", jobID, processID, fileName);

        return true;
    }

    if (!readBinary(desc, Util::getAbsRefPath(getHost().getRootPath(), jobID, fileName).c_str(),
                    block.get(1), crc)) {
        LOGS_E(getHost(), "readFile can not read Binary data");
        return false;
    }

    LOGS_D(getHost(), "File Binary is read successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
           id, state? "output" : "input", jobID, processID, fileName);

    return true;
}

bool Message::readMessageBlock(int in, MessageBlockHeader &block, uint32_t& crc) {

    switch(block.getType()) {

        case BLOCK_PROCESSID: {

            long processID;
            if (!readProcessID(in, processID, block, crc)) {
                return false;
            }

            data.setFileProcess(processID);
        }
            break;

        case BLOCK_FILEINFO:
        case BLOCK_FILEBINARY: {

            ProcessFile processFile(new FileItem(getHost()));

            if (!readFile(in, processFile, block, crc)) {
                return false;
            }

            data.addFile(processFile);
        }
            break;

        case BLOCK_JOB: {

            char jobName[NAME_MAX];
            if (!readJobName(in, jobName, block, crc)) {
                return false;
            }

            data.setJobName(jobName);
        }

            break;

        case BLOCK_PROCESSINFO: {

            auto *processItem = new ProcessItem(getHost());

            if (!readProcess(in, processItem, block, crc)) {
                return false;
            }

            data.addProcess(processItem);
        }

            break;

        case BLOCK_COMPONENT: {

            if (!readComponentList(in, data.getComponentList(), block, crc)) {
                return false;
            }

            break;
        }

        default:
            return false;
    }

    return true;
}

bool Message::writeComponentList(int desc, std::vector<ComponentObject>& componentList, uint32_t& crc) {

    std::vector<long> list;

    for (auto &component : componentList) {
        list.emplace_back(component.getID());
        list.emplace_back(component.getAddress());
    }

    MessageBlockHeader blockHeader(BLOCK_COMPONENT);

    blockHeader.add(componentList[0].getType());
    blockHeader.add(list.size());

    if (!writeBlockHeader(desc, blockHeader, crc)) {
        LOGS_E(getHost(), "writeComponentList can not write block header");
        return false;
    }

    if (!writeNumberList(desc, list, crc)) {
        LOGS_E(getHost(), "writeNumberList can not write component list");
        return false;
    }

    LOGS_D(getHost(), "Component list is written successfully => Count : %d", componentList.size());
    for (int i = 0; i < componentList.size(); i++) {
        LOGS_D(getHost(), "Component[%d] : %d", i, componentList[0].getID());
    }

    return true;
}

bool Message::writeJobName(int desc, const char *jobName, uint32_t& crc) {

    MessageBlockHeader blockHeader(BLOCK_JOB);

    blockHeader.add(strlen(jobName));

    if (!writeBlockHeader(desc, blockHeader, crc)) {
        LOGS_E(getHost(), "writeJobName can not write block header");
        return false;
    }

    if (!writeString(desc, jobName, crc)) {
        LOGS_E(getHost(), "writeJobName can not write job info");
        return false;
    }

    LOGS_D(getHost(), "Job Name is written successfully => Name : %s", jobName);

    return true;
}

bool Message::writeProcessID(int desc, long processID, uint32_t& crc) {

    MessageBlockHeader blockHeader(BLOCK_PROCESSID);

    if (!writeBlockHeader(desc, blockHeader, crc)) {
        LOGS_E(getHost(), "writeProcessID can not write block header");
        return false;
    }

    if (!writeNumber(desc, processID, crc)) {
        LOGS_E(getHost(), "writeProcess can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process ID is written successfully => ID : %ld", processID);

    return true;
}

bool Message::writeProcess(int desc, ProcessItem* processItem, uint32_t& crc) {

    MessageBlockHeader blockHeader(BLOCK_PROCESSINFO);

    blockHeader.add(strlen(processItem->getParsedProcess()));

    if (!writeBlockHeader(desc, blockHeader, crc)) {
        LOGS_E(getHost(), "writeProcess can not write block header");
        return false;
    }

    if (!writeNumber(desc, processItem->getID(), crc)) {
        LOGS_E(getHost(), "writeProcess can not write process ID");
        return false;
    }

    if (!writeNumber(desc, processItem->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeProcess can not write job ID");
        return false;
    }

    if (!writeString(desc, processItem->getParsedProcess(), crc)) {
        LOGS_E(getHost(), "writeProcess can not write process info");
        return false;
    }

    LOGS_D(getHost(), "Process is written successfully => ID : %ld, jobID : %ld, Process : %s",
           processItem->getID(), processItem->getAssignedJob(), processItem->getParsedProcess());

    return true;
}

bool Message::writeFile(int desc, ProcessFile &content, bool isBinary, uint32_t& crc) {

    std::string absPath = Util::getAbsRefPath(content.get()->getHost().getRootPath(),
            content.get()->getAssignedJob(), content.get()->getName());

    bool isBinaryTransfer = !content.isOutput() && isBinary;

    MessageBlockHeader blockHeader(!isBinaryTransfer ? BLOCK_FILEINFO : BLOCK_FILEBINARY);

    blockHeader.add(strlen(content.get()->getName()));
    blockHeader.add(content.get()->getSize());

	if (!writeBlockHeader(desc, blockHeader, crc)) {
        LOGS_E(getHost(), "writeFile can not write block header");
		return false;
	}

    if (!writeNumber(desc, content.get()->getID(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file ID");
        return false;
    }

    if (!writeNumber(desc, content.isOutput(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file state");
        return false;
    }

    if (!writeNumber(desc, content.get()->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeFile can not write assigned job");
        return false;
    }

    if (!writeNumber(desc, content.getAssignedProcess(), crc)) {
        LOGS_E(getHost(), "writeFile can not write assigned job");
        return false;
    }

	if (!writeString(desc, content.get()->getName(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file name");
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeBinary(desc, absPath.c_str(), content.get()->getSize(), crc)) {
            LOGS_E(getHost(), "writeFile can not write Binary data");
            return false;
        }

        LOGS_D(getHost(), "File Binary is written successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
               content.get()->getID(), content.isOutput()? "output" : "input",
               content.get()->getAssignedJob(), content.getAssignedProcess(), content.get()->getName());

        return true;
    }

    LOGS_D(getHost(), "File Info is written successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
           content.get()->getID(), content.isOutput()? "output" : "input",
           content.get()->getAssignedJob(), content.getAssignedProcess(), content.get()->getName());

	return true;

}

bool Message::writeMessageStream(int out, uint32_t& crc) {

    switch(data.getStreamFlag()) {

        case STREAM_PROCESS:

            for (int i = 0; i < data.getProcessCount(); i++) {

                ProcessItem* processItem = data.getProcess(i);

                if (!writeProcess(out, processItem, crc)) {
                    return false;
                }

                for (auto processFile : processItem->getFileList()) {

                    if (!writeFile(out, processFile, false, crc)) {
                        return false;
                    }
                }
            }

            break;

        case STREAM_FILEINFO:
        case STREAM_FILEBINARY:

            if (!writeProcessID(out, data.getFileProcess(), crc)) {
                return false;
            }

            for (int i = 0; i < data.getFileCount(); i++) {

                if (!writeFile(out, data.getFile(i), data.getStreamFlag() == STREAM_FILEBINARY, crc)) {
                    return false;
                }
            }

            break;

        case STREAM_JOB:

            if (!writeJobName(out, data.getJobName(), crc)) {
                return false;
            }

            break;

        case STREAM_COMPONENT: {

            if (!writeComponentList(out, data.getComponentList(), crc)) {
                return false;
            }

            break;
        }

        case STREAM_NONE:
        default :
            break;
    }

    return true;

}

MessageHeader& Message::getHeader() {
    return header;
}

bool Message::deSerializeHeader(const uint8_t *buffer) {
    return header.deSerialize(buffer);
}

bool Message::serializeHeader(uint8_t *buffer) {
    return header.serialize(buffer);
}

long Message::getHeaderSize() {
    return header.getSize();
}

MessageData& Message::getData() {
    return data;
}
