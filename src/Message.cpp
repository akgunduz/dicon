//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "Util.h"
#include "ComponentUnitFactory.h"

Message::Message(const TypeHostUnit& host)
		: MessageBase(host) {

    header.setPriority(MESSAGE_DEFAULT_PRIORITY);

    data.setStreamFlag(STREAM_NONE);

}

Message::Message(const TypeHostUnit& host, const TypeComponentUnit& target, MSG_TYPE type)
		: MessageBase(host) {

    header.setType(type);
    header.setOwner(host->getUnit(target->getType()));
    header.setPriority(MESSAGE_DEFAULT_PRIORITY);

    data.setStreamFlag(STREAM_NONE);
}

Message::~Message() = default;

bool Message::readComponentList(const TypeComponentUnit& source, TypeComponentUnitList &componentList,
                                MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_COMPONENT) {
        LOGS_E(getHost(), "readComponentList can not read other blocks");
        return false;
    }

    std::vector<long> list;

    if (!readNumberList(source, list, block.get(1), crc)) {
        LOGS_E(getHost(), "readNumberList can not read number");
        return false;
    }

    for (int i = 0; i < list.size(); i = i + 6) {

        Address address(BaseAddress(list[i + 2], list[i + 3]),
                        BaseAddress(list[i + 4], list[i + 5]));

        componentList.emplace_back(ComponentUnitFactory::create((COMPONENT)block.get(0),
                                                                (ARCH)list[i], list[i + 1], address));
    }

    LOGS_D(getHost(), "Component list is read successfully => Count : %d", componentList.size());
    for (auto &item : componentList) {
        LOGS_D(getHost(), "Component[%s] : %d", ComponentType::getName(item->getType()), item->getID());
    }

    return true;
}

bool Message::readJobName(const TypeComponentUnit& source, char *jobName, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_JOB) {
        LOGS_E(getHost(), "readJobName can not read other blocks");
        return false;
    }

    if (!readString(source, jobName, block.get(0), crc)) {
        LOGS_E(getHost(), "readJobName can not read job name");
        return false;
    }

    LOGS_D(getHost(), "Job Name is read successfully => Name : %s", jobName);

    return true;
}

bool Message::readProcessID(const TypeComponentUnit& source, long& processID, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_PROCESSID) {
        LOGS_E(getHost(), "readJobName can not read other blocks");
        return false;
    }

    if (!readNumber(source, processID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }

    LOGS_D(getHost(), "Process ID is read successfully => ID : %ld", processID);

    return true;
}

bool Message::readProcess(const TypeComponentUnit& source, const TypeProcessItem& content,
                          MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_PROCESSINFO) {
        LOGS_E(getHost(), "readProcessInfo can not read other blocks");
        return false;
    }

    long id;
    if (!readNumber(source, id, crc)) {
        LOGS_E(getHost(), "readProcessInfo can not read process id");
        return false;
    }
    content->setID(id);

    long jobID;
    if (!readNumber(source, jobID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }
    content->setAssignedJob(jobID);

    char process[PATH_MAX];
    if (!readString(source, process, block.get(0), crc)) {
        LOGS_E(getHost(), "readProcessInfo can not read process info");
        return false;
    }
    content->setParsedProcess(process);

    LOGS_D(getHost(), "Process is read successfully => ID : %ld, jobID : %ld, Process : %s",
            id, jobID, process);

    return true;
}

bool Message::readFile(const TypeComponentUnit& source, ProcessFile& content, MessageBlockHeader &block, uint32_t& crc) {

    if (block.getType() != BLOCK_FILEBINARY && block.getType() != BLOCK_FILEINFO) {
        LOGS_E(getHost(), "readFile can not read other blocks");
        return false;
    }

    long id;
    if (!readNumber(source, id, crc)) {
        LOGS_E(getHost(), "readFile can not read id data");
        return false;
    }
    content.get()->setID(id);

    long state;
    if (!readNumber(source, state, crc)) {
        LOGS_E(getHost(), "readFile can not read state data");
        return false;
    }
    content.setOutputState((bool)state);

    long jobID;
    if (!readNumber(source, jobID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }
    content.get()->setAssignedJob(jobID);

    long processID;
    if (!readNumber(source, processID, crc)) {
        LOGS_E(getHost(), "readFile can not read jobID data");
        return false;
    }
    content.setAssignedProcess(processID);

    char fileName[PATH_MAX];
    if (!readString(source, fileName, block.get(0), crc)) {
        LOGS_E(getHost(), "readFile can not read path data");
        return false;
    }
    content.get()->setName(fileName);

    if (block.getType() == BLOCK_FILEINFO || content.isOutput()) {

        LOGS_D(getHost(), "File Info is read successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
               id, state? "output" : "input", jobID, processID, fileName);

        return true;
    }

    std::filesystem::path filePath = getHost()->getRootPath() / std::to_string(jobID) / fileName;
    if (!readBinary(source, filePath.c_str(), block.get(1), crc)) {
        LOGS_E(getHost(), "readFile can not read Binary data");
        return false;
    }

    LOGS_D(getHost(), "File Binary is read successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
           id, state? "output" : "input", jobID, processID, fileName);

    return true;
}

bool Message::readMessageBlock(const TypeComponentUnit& source, MessageBlockHeader &block, uint32_t& crc) {

    switch(block.getType()) {

        case BLOCK_PROCESSID: {

            long processID;
            if (!readProcessID(source, processID, block, crc)) {
                return false;
            }

            data.setFileProcess(processID);
        }
            break;

        case BLOCK_FILEINFO:
        case BLOCK_FILEBINARY: {

            ProcessFile processFile(std::make_shared<FileItem>(getHost()));

            if (!readFile(source, processFile, block, crc)) {
                return false;
            }

            data.addFile(processFile);
        }
            break;

        case BLOCK_JOB: {

            char jobName[NAME_MAX];
            if (!readJobName(source, jobName, block, crc)) {
                return false;
            }

            data.setJobName(jobName);
        }

            break;

        case BLOCK_PROCESSINFO: {

            auto processItem = std::make_shared<ProcessItem>(getHost());

            if (!readProcess(source, processItem, block, crc)) {
                return false;
            }

            data.addProcess(processItem);
        }

            break;

        case BLOCK_COMPONENT: {

            if (!readComponentList(source, data.getComponentList(), block, crc)) {
                return false;
            }

            break;
        }

        default:
            return false;
    }

    return true;
}

bool Message::writeComponentList(const TypeComponentUnit& target, TypeComponentUnitList& componentList, uint32_t& crc) {

    std::vector<long> list;

    for (auto &component : componentList) {

        list.emplace_back(component->getArch());
        list.emplace_back(component->getID());
        list.emplace_back(component->getAddress().get().base);
        list.emplace_back(component->getAddress().get().port);
        list.emplace_back(component->getAddress().getUI().base);
        list.emplace_back(component->getAddress().getUI().port);
    }

    MessageBlockHeader blockHeader(BLOCK_COMPONENT);

    blockHeader.add(componentList[0]->getType());
    blockHeader.add(list.size());

    if (!writeBlockHeader(target, blockHeader, crc)) {
        LOGS_E(getHost(), "writeComponentList can not write block header");
        return false;
    }

    if (!writeNumberList(target, list, crc)) {
        LOGS_E(getHost(), "writeNumberList can not write component list");
        return false;
    }

    LOGS_D(getHost(), "Component list is written successfully => Count : %d", componentList.size());
    for (auto& item : componentList) {
        LOGS_D(getHost(), "Component[%s] : %d", ComponentType::getName(item->getType()), item->getID());
    }

    return true;
}

bool Message::writeJobName(const TypeComponentUnit& target, const std::string& jobName, uint32_t& crc) {

    MessageBlockHeader blockHeader(BLOCK_JOB);

    blockHeader.add(jobName.size());

    if (!writeBlockHeader(target, blockHeader, crc)) {
        LOGS_E(getHost(), "writeJobName can not write block header");
        return false;
    }

    if (!writeString(target, jobName, crc)) {
        LOGS_E(getHost(), "writeJobName can not write job info");
        return false;
    }

    LOGS_D(getHost(), "Job Name is written successfully => Name : %s", jobName.c_str());

    return true;
}

bool Message::writeProcessID(const TypeComponentUnit& target, long processID, uint32_t& crc) {

    MessageBlockHeader blockHeader(BLOCK_PROCESSID);

    if (!writeBlockHeader(target, blockHeader, crc)) {
        LOGS_E(getHost(), "writeProcessID can not write block header");
        return false;
    }

    if (!writeNumber(target, processID, crc)) {
        LOGS_E(getHost(), "writeProcess can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process ID is written successfully => ID : %ld", processID);

    return true;
}

bool Message::writeProcess(const TypeComponentUnit& target, const TypeProcessItem& processItem, uint32_t& crc) {

    MessageBlockHeader blockHeader(BLOCK_PROCESSINFO);

    blockHeader.add(processItem->getParsedProcess().size());

    if (!writeBlockHeader(target, blockHeader, crc)) {
        LOGS_E(getHost(), "writeProcess can not write block header");
        return false;
    }

    if (!writeNumber(target, processItem->getID(), crc)) {
        LOGS_E(getHost(), "writeProcess can not write process ID");
        return false;
    }

    if (!writeNumber(target, processItem->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeProcess can not write job ID");
        return false;
    }

    if (!writeString(target, processItem->getParsedProcess(), crc)) {
        LOGS_E(getHost(), "writeProcess can not write process info");
        return false;
    }

    LOGS_D(getHost(), "Process is written successfully => ID : %ld, jobID : %ld, Process : %s",
           processItem->getID(), processItem->getAssignedJob(), processItem->getParsedProcess().c_str());

    return true;
}

bool Message::writeFile(const TypeComponentUnit& target, ProcessFile &content, bool isBinary, uint32_t& crc) {

    std::filesystem::path filePath = content.get()->getHost()->getRootPath() /
            std::to_string(content.get()->getAssignedJob()) / content.get()->getName();

    bool isBinaryTransfer = !content.isOutput() && isBinary;

    MessageBlockHeader blockHeader(!isBinaryTransfer ? BLOCK_FILEINFO : BLOCK_FILEBINARY);

    blockHeader.add(content.get()->getName().size());
    blockHeader.add(content.get()->getSize());

	if (!writeBlockHeader(target, blockHeader, crc)) {
        LOGS_E(getHost(), "writeFile can not write block header");
		return false;
	}

    if (!writeNumber(target, content.get()->getID(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file ID");
        return false;
    }

    if (!writeNumber(target, content.isOutput(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file state");
        return false;
    }

    if (!writeNumber(target, content.get()->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeFile can not write assigned job");
        return false;
    }

    if (!writeNumber(target, content.getAssignedProcess(), crc)) {
        LOGS_E(getHost(), "writeFile can not write assigned job");
        return false;
    }

	if (!writeString(target, content.get()->getName(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file name");
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeBinary(target, filePath.c_str(), content.get()->getSize(), crc)) {
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

bool Message::writeMessageStream(const TypeComponentUnit& target, uint32_t& crc) {

    switch(data.getStreamFlag()) {

        case STREAM_PROCESS:

            for (int i = 0; i < data.getProcessCount(); i++) {

                auto processItem = data.getProcess(i);

                if (!writeProcess(target, processItem, crc)) {
                    return false;
                }

                for (auto processFile : processItem->getFileList()) {

                    if (!writeFile(target, processFile, false, crc)) {
                        return false;
                    }
                }
            }

            break;

        case STREAM_FILEINFO:
        case STREAM_FILEBINARY:

            if (!writeProcessID(target, data.getFileProcess(), crc)) {
                return false;
            }

            for (int i = 0; i < data.getFileCount(); i++) {

                if (!writeFile(target, data.getFile(i), data.getStreamFlag() == STREAM_FILEBINARY, crc)) {
                    return false;
                }
            }

            break;

        case STREAM_JOB:

            if (!writeJobName(target, data.getJobName(), crc)) {
                return false;
            }

            break;

        case STREAM_COMPONENT: {

            if (!writeComponentList(target, data.getComponentList(), crc)) {
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

void Message::deSerializeHeader(const uint8_t *buffer) {
    header.deSerialize(buffer);
}

void Message::grabOwner(const TypeCommUnit& unit) {
    header.grabOwner(unit);
}

void Message::serializeHeader(uint8_t *buffer) {
    header.serialize(buffer);
}

long Message::getHeaderSize() {
    return header.getSize();
}

MessageData& Message::getData() {
    return data;
}
