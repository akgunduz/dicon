//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "ComponentUnitFactory.h"

Message::Message(const TypeHostUnit& host)
		: MessageBase(host), data(host) {
}

Message::Message(const TypeHostUnit& host, const TypeComponentUnit& target, MSG_TYPE msgType)
		: MessageBase(host, target, msgType), data(host) {

}

bool Message::readComponentList(const TypeComponentUnit& source, TypeComponentUnitList &componentList,
                                MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_COMPONENT) {
        LOGS_E(getHost(), "readComponentList can not read other blocks");
        return false;
    }

    std::vector<uint64_t> list;

    if (!readNumberList(source, list, block.get(1), crc)) {
        LOGS_E(getHost(), "readNumberList can not read number");
        return false;
    }

    for (uint32_t i = 0; i < list.size(); i = i + 6) {

        Address address(BaseAddress((uint32_t)list[i + 2], (uint16_t)list[i + 3]),
                        BaseAddress((uint32_t)list[i + 4], (uint16_t)list[i + 5]));

        componentList.emplace_back(ComponentUnitFactory::create((COMPONENT)block.get(0),
                                                                (ARCH)list[i], list[i + 1], address));
    }

    LOGS_D(getHost(), "Component list is read successfully => Count : %d", componentList.size());
    for (auto &item : componentList) {
        LOGS_D(getHost(), "Component[%s] : %d", ComponentType::getName(item->getType()), item->getID());
    }

    return true;
}

bool Message::readNumberList(const TypeComponentUnit& source, std::vector<uint64_t> &list, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "NumberList read process is started, count : %ld", size);

//    for (size_t i = 0; i < size; i++) {
//
//        if (!readBlock(source, tmpBuf, 8, crc)) {
//            LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read number from stream");
//            return false;
//        }
//
//        list.emplace_back(ntohll(*((uint64_t*) tmpBuf)));
//    }

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "NumberList is read successfully, count : %ld, first number => %ld", size, list[0]);

    return true;
}

bool Message::readJobName(const TypeComponentUnit& source, std::string& jobName, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_JOB) {
        LOGS_E(getHost(), "readJobName can not read other blocks");
        return false;
    }

//    if (!readString(source, jobName, block.get(0), crc)) {
//        LOGS_E(getHost(), "readJobName can not read job name");
//        return false;
//    }

    LOGS_D(getHost(), "Job Name is read successfully => Name : %s", jobName.c_str());

    return true;
}

bool Message::readProcessID(const TypeComponentUnit& source, long& processID, MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_PROCESSID) {
        LOGS_E(getHost(), "readJobName can not read other blocks");
        return false;
    }

    uint64_t number;
//    if (!readNumber(source, number, crc)) {
//        LOGS_E(getHost(), "readFile can not read jobID data");
//        return false;
//    }

    processID = (long)number;

    LOGS_D(getHost(), "Process ID is read successfully => ID : %ld", processID);

    return true;
}

bool Message::readProcess(const TypeComponentUnit& source, const TypeProcessItem& content,
                          MessageBlockHeader& block, uint32_t& crc) {

    if (block.getType() != BLOCK_PROCESSINFO) {
        LOGS_E(getHost(), "readProcessInfo can not read other blocks");
        return false;
    }

    uint64_t id;
//    if (!readNumber(source, id, crc)) {
//        LOGS_E(getHost(), "readProcessInfo can not read process id");
//        return false;
//    }
    content->setID((long)id);

    uint64_t jobID;
//    if (!readNumber(source, jobID, crc)) {
//        LOGS_E(getHost(), "readFile can not read jobID data");
//        return false;
//    }
    content->setAssignedJob((long)jobID);

    std::string process;
//    if (!readString(source, process, block.get(0), crc)) {
//        LOGS_E(getHost(), "readProcessInfo can not read process info");
//        return false;
//    }
    content->setParsedProcess(process);

    LOGS_D(getHost(), "Process is read successfully => ID : %ld, jobID : %ld, Process : %s",
            id, jobID, process.c_str());

    return true;
}

bool Message::readFile(const TypeComponentUnit& source, const TypeProcessFile& content, MessageBlockHeader &block, uint32_t& crc) {

    if (block.getType() != BLOCK_FILEBINARY && block.getType() != BLOCK_FILEINFO) {
        LOGS_E(getHost(), "readFile can not read other blocks");
        return false;
    }

    uint64_t id;
//    if (!readNumber(source, id, crc)) {
//        LOGS_E(getHost(), "readFile can not read id data");
//        return false;
//    }
    content->get()->setID((long)id);

    uint64_t state;
//    if (!readNumber(source, state, crc)) {
//        LOGS_E(getHost(), "readFile can not read state data");
//        return false;
//    }
    content->setOutputState((bool)state);

    uint64_t jobID;
//    if (!readNumber(source, jobID, crc)) {
//        LOGS_E(getHost(), "readFile can not read jobID data");
//        return false;
//    }
    content->get()->setAssignedJob((long)jobID);

    uint64_t processID;
//    if (!readNumber(source, processID, crc)) {
//        LOGS_E(getHost(), "readFile can not read jobID data");
//        return false;
//    }
    content->setAssignedProcess((long)processID);

    std::string fileName;
//    if (!readString(source, fileName, block.get(0), crc)) {
//        LOGS_E(getHost(), "readFile can not read path data");
//        return false;
//    }
    content->get()->setName(fileName);

    if (block.getType() == BLOCK_FILEINFO || content->isOutput()) {

        LOGS_D(getHost(), "File Info is read successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
               id, state? "output" : "input", jobID, processID, fileName.c_str());

        return true;
    }

    std::filesystem::path filePath = getHost()->getRootPath() / std::to_string(jobID) / fileName;
//    if (!readBinary(source, filePath.c_str(), block.get(1), crc)) {
//        LOGS_E(getHost(), "readFile can not read Binary data");
//        return false;
//    }

    LOGS_D(getHost(), "File Binary is read successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
           id, state? "output" : "input", jobID, processID, fileName.c_str());

    return true;
}

bool Message::readMessageBlock(const TypeComponentUnit& source, MessageBlockHeader &block, uint32_t& crc) {

    switch(block.getType()) {

        case BLOCK_PROCESSINFO: {

            if (!readProcess(source, data.getProcess(), block, crc)) {
                return false;
            }
        }
            break;

        case BLOCK_PROCESSID: {

            long processID;

            if (!readProcessID(source, processID, block, crc)) {
                return false;
            }

            data.getProcess()->setID(processID);
        }
            break;

        case BLOCK_FILEINFO:
        case BLOCK_FILEBINARY: {

            auto processFile = std::make_shared<ProcessFile>(std::make_shared<FileItem>(getHost()));

            if (!readFile(source, processFile, block, crc)) {

                return false;
            }

            if (processFile->getAssignedProcess() != data.getProcess()->getID()) {

                LOGS_E(getHost(), "Mismatch ID with process and associated file, process : %d, file : %d",
                       data.getProcess()->getID(), processFile->getAssignedProcess());
                return false;
            }

            data.getProcess()->addFile(processFile);
        }
            break;

        case BLOCK_JOB: {

            if (!readJobName(source, data.getJobName(), block, crc)) {
                return false;
            }
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

    std::vector<uint64_t> list;

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

bool Message::writeNumberList(const TypeComponentUnit& target, std::vector<uint64_t>& list, uint32_t& crc) {

    uint8_t buffer[8] = {};

    LOGC_T(getHost(), target, MSGDIR_SEND, "NumberList write process is started  => Count : %ld, First Number : %lld",
           list.size(), list[0]);

    for (auto number : list) {

        *((uint64_t *) buffer) = htonll(number);
        if (!writeBlock(target, buffer, 8, crc)) {
            LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write char array to stream");
            return false;
        }
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "NumberList is written successfully => Count : %ld, First Number : %lld",
           list.size(), list[0]);

    return true;
}

bool Message::writeJobName(const TypeComponentUnit& target, const std::string& jobName, uint32_t& crc) {

    if (!writeString(target, jobName, crc)) {
        LOGS_E(getHost(), "writeJobName can not write job info");
        return false;
    }

    LOGS_D(getHost(), "Job Name is written successfully => Name : %s", jobName.c_str());

    return true;
}

bool Message::writeFileBinary(const TypeComponentUnit& target, const TypeFileItem& file, uint32_t& crc) {

    if (!writeString(target, file->getName(), crc)) {
        LOGS_E(getHost(), "writeFileBinary can not write file path");
        return false;
    }

    if (!writeBinary(target, file->getPath() / file->getName(), file->getSize(), crc)) {
        LOGS_E(getHost(), "writeFileBinary can not write file binary");
        return false;
    }

    LOGS_D(getHost(), "File Binary is written successfully => Name : %s", file->getName().c_str());

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

bool Message::writeFile(const TypeComponentUnit& target, const TypeProcessFile& content, bool isBinary, uint32_t& crc) {

    std::filesystem::path filePath = content->get()->getHost()->getRootPath() /
            std::to_string(content->get()->getAssignedJob()) / content->get()->getName();

    bool isBinaryTransfer = !content->isOutput() && isBinary;

    MessageBlockHeader blockHeader(!isBinaryTransfer ? BLOCK_FILEINFO : BLOCK_FILEBINARY);

    blockHeader.add(content->get()->getName().size());
    blockHeader.add(content->get()->getSize());

	if (!writeBlockHeader(target, blockHeader, crc)) {
        LOGS_E(getHost(), "writeFile can not write block header");
		return false;
	}

    if (!writeNumber(target, content->get()->getID(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file ID");
        return false;
    }

    if (!writeNumber(target, content->isOutput(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file state");
        return false;
    }

    if (!writeNumber(target, content->get()->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeFile can not write assigned job");
        return false;
    }

    if (!writeNumber(target, content->getAssignedProcess(), crc)) {
        LOGS_E(getHost(), "writeFile can not write assigned job");
        return false;
    }

	if (!writeString(target, content->get()->getName(), crc)) {
        LOGS_E(getHost(), "writeFile can not write file name");
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeBinary(target, filePath.c_str(), content->get()->getSize(), crc)) {
            LOGS_E(getHost(), "writeFile can not write Binary data");
            return false;
        }

        LOGS_D(getHost(), "File Binary is written successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
               content->get()->getID(), content->isOutput()? "output" : "input",
               content->get()->getAssignedJob(), content->getAssignedProcess(), content->get()->getName().c_str());

        return true;
    }

    LOGS_D(getHost(), "File Info is written successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
           content->get()->getID(), content->isOutput()? "output" : "input",
           content->get()->getAssignedJob(), content->getAssignedProcess(), content->get()->getName().c_str());

	return true;

}

bool Message::writeMessageStream(const TypeComponentUnit& target, uint32_t& crc) {

    switch(data.getStreamType()) {

        case STREAM_PROCESS:

            if (!writeProcess(target, data.getProcess(), crc)) {
                return false;
            }

            for (const auto& processFile : data.getProcess()->getFileList()) {

                if (!writeFile(target, processFile, false, crc)) {
                    return false;
                }
            }

            break;

        case STREAM_FILEINFO:
        case STREAM_FILEBINARY:

            if (!writeProcessID(target, data.getProcess()->getID(), crc)) {
                return false;
            }

            for (const auto& processFile : data.getProcess()->getFileList()) {

                if (!writeFile(target, processFile, data.getStreamType() == STREAM_FILEBINARY, crc)) {
                    return false;
                }
            }

            break;

        case STREAM_FILESOLO:

            if (!writeFileBinary(target, data.getFile(), crc)) {
                return false;
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

MessageData& Message::getData() {

    return data;
}
