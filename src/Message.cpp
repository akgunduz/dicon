//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "ComponentUnitFactory.h"

Message::Message(const TypeHostUnit& host)
		: MessageBase(host), data(host) {
}

Message::Message(const TypeHostUnit& host, const TypeComponentUnit& target, MSG_TYPE msgType, STREAM_TYPE streamType)
		: MessageBase(host, target, msgType, streamType), data(host) {

}

bool Message::readJobName(const TypeComponentUnit& source, std::string& jobName) {

    jobName = strings.front();
    strings.pop_front();

    LOGS_D(getHost(), "Job Name is read successfully => Name : %s", jobName.c_str());

    return true;
}

bool Message::readFileBinary(const TypeComponentUnit& source, const TypeFileItem& fileItem) {

    fileItem->setName(strings.front());
    strings.pop_front();

    bool status = fileItem->check();

    LOGS_D(getHost(), "File is read successfully => Name : %s, Size : %ld, Health : %s",
           fileItem->getName().c_str(), fileItem->getSize(), status ? "OK" : "NOK");

    return true;
}

bool Message::readProcessID(const TypeComponentUnit& source, const TypeProcessItem& processItem) {

    processItem->setID((long)numbers.front());
    numbers.pop_front();

    LOGS_D(getHost(), "Process[%ld] is read successfully", processItem->getID());

    return true;
}

bool Message::readProcessInfo(const TypeComponentUnit& source, const TypeProcessItem& processItem) {

    processItem->setID((long)numbers.front());
    numbers.pop_front();

    processItem->setAssignedJob((long)numbers.front());
    numbers.pop_front();

    processItem->setParsedProcess(strings.front());
    strings.pop_front();

    LOGS_D(getHost(), "Process[%ld] is read successfully => jobID : %ld, Process : %s",
           processItem->getID(), processItem->getAssignedJob(),
           processItem->getParsedProcess().c_str());

    return true;
}

bool Message::readProcessFileCount(const TypeComponentUnit& source, long& count) {

    count = numbers.front();
    numbers.pop_front();

    LOGS_D(getHost(), "Process has %ld files", count);

    return true;
}

bool Message::readProcessFile(const TypeComponentUnit& source, const TypeProcessFile& processFile) {

    processFile->get()->setID((long)numbers.front());
    numbers.pop_front();

    processFile->setOutputState((bool)numbers.front());
    numbers.pop_front();

    processFile->get()->setAssignedJob((long)numbers.front());
    numbers.pop_front();

    processFile->setAssignedProcess((long)numbers.front());
    numbers.pop_front();

    processFile->get()->setName(strings.front());
    strings.pop_front();

    LOGS_D(getHost(), "Process[%d] File[%d] is read successfully => State : %s, Job : %d, Name : %s",
           processFile->getAssignedProcess(), processFile->get()->getID(),
           processFile->isOutput() ? "output" : "input", processFile->get()->getAssignedJob(),
           processFile->get()->getName().c_str());

    return true;
}

bool Message::readProcessFiles(const TypeComponentUnit& source, const TypeProcessItem& processItem) {

    readProcessID(source, processItem);

    long fileCount;
    readProcessFileCount(source, fileCount);

    for (long i = 0; i < fileCount; i++) {

        auto processFile = std::make_shared<ProcessFile>(std::make_shared<FileItem>(getHost()));

        readProcessFile(source, processFile);

        if (processFile->getAssignedProcess() != processItem->getID()) {

            LOGS_E(getHost(), "Mismatch ID with process and associated file, process : %d, file : %d",
                   processItem->getID(), processFile->getAssignedProcess());

            return false;
        }

        processItem->addFile(processFile);
    }

    return true;
}




bool Message::readComponentList(const TypeComponentUnit& source, TypeComponentUnitList &componentList,
                                MessageBlockHeader& block, uint32_t& crc) {

    //TODO NOT PROCESSED
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

    //TODO NOT PROCESSED
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







bool Message::build(const TypeComponentUnit& source) {

//    if (!writeProcess(target, data.getProcess(), crc)) {
//        return false;
//    }
//
//    for (const auto& processFile : data.getProcess()->getFileList()) {
//
//        if (!writeFile(target, processFile, false, crc)) {
//            return false;
//        }
//    }

    switch(getHeader().getStream()) {

        case STREAM_JOBNAME:

            readJobName(source, data.getJobName());

            break;

        case STREAM_FILEBINARY:

            readFileBinary(source, data.getFile());

            break;

        case STREAM_PROCESSID:

            readProcessID(source, data.getProcess());

            break;

        case STREAM_PROCESSINFO:

            readProcessInfo(source, data.getProcess());

            break;

        case STREAM_PROCESSFILEINFO:
        case STREAM_PROCESSFILEBINARY:

            readProcessFile(source, data.getProcessFile());

            break;

        case STREAM_PROCESSFILEINFO_ALL:
        case STREAM_PROCESSFILEBINARY_ALL:

            readProcessFiles(source, data.getProcess());

            break;

        default:
            break;
    }

    return true;
}

bool Message::readMessageBlock(const TypeComponentUnit& source, MessageBlockHeader &block, uint32_t& crc) {

//    switch(block.getType()) {
//
//        case BLOCK_PROCESSINFO: {
//
//            if (!readProcess(source, data.getProcess(), crc)) {
//                return false;
//            }
//        }
//            break;
//
//        case BLOCK_PROCESSID: {
//
//            long processID;
//
//            if (!readProcessID(source, processID, block, crc)) {
//                return false;
//            }
//
//            data.getProcess()->setID(processID);
//        }
//            break;
//
//        case BLOCK_FILEINFO:
//        case BLOCK_FILEBINARY: {
//
//            auto processFile = std::make_shared<ProcessFile>(std::make_shared<FileItem>(getHost()));
//
//            if (!readFile(source, processFile, block, crc)) {
//
//                return false;
//            }
//
//            if (processFile->getAssignedProcess() != data.getProcess()->getID()) {
//
//                LOGS_E(getHost(), "Mismatch ID with process and associated file, process : %d, file : %d",
//                       data.getProcess()->getID(), processFile->getAssignedProcess());
//                return false;
//            }
//
//            data.getProcess()->addFile(processFile);
//        }
//            break;
//
//        case BLOCK_JOB: {
//
//            if (!readJobName(source, data.getJobName(), block, crc)) {
//                return false;
//            }
//        }
//            break;
//
//        case BLOCK_COMPONENT: {
//
//            if (!readComponentList(source, data.getComponentList(), block, crc)) {
//                return false;
//            }
//
//            break;
//        }
//
//        default:
//            return false;
//    }

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

    if (!writeString(target, file->getRefPath(), crc)) {
        LOGS_E(getHost(), "writeFileBinary can not write file path");
        return false;
    }

    if (!writeBinary(target, file->getPath(), file->getSize(), crc)) {
        LOGS_E(getHost(), "writeFileBinary can not write file binary");
        return false;
    }

    LOGS_D(getHost(), "File Binary is written successfully => Name : %s", file->getName().c_str());

    return true;
}

bool Message::writeProcessID(const TypeComponentUnit& target, const TypeProcessItem& processItem, uint32_t& crc) {

    if (!writeNumber(target, processItem->getID(), crc)) {
        LOGS_E(getHost(), "writeProcessID can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process ID is written successfully => ID : %ld", processItem->getID());

    return true;
}

bool Message::writeProcessInfo(const TypeComponentUnit& target, const TypeProcessItem& processItem, uint32_t& crc) {

    if (!writeNumber(target, processItem->getID(), crc)) {
        LOGS_E(getHost(), "writeProcessInfo can not write process ID");
        return false;
    }

    if (!writeNumber(target, processItem->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeProcessInfo can not write job ID");
        return false;
    }

    if (!writeString(target, processItem->getParsedProcess(), crc)) {
        LOGS_E(getHost(), "writeProcessInfo can not write process info");
        return false;
    }

    LOGS_D(getHost(), "Process Info is written successfully => ID : %ld, jobID : %ld, Process : %s",
           processItem->getID(), processItem->getAssignedJob(), processItem->getParsedProcess().c_str());

    return true;
}

bool Message::writeProcessFileCount(const TypeComponentUnit& target,
                                    const TypeProcessItem& processItem, uint32_t& crc) {

    if (!writeNumber(target, processItem->getFileCount(), crc)) {
        LOGS_E(getHost(), "writeProcessFileCount can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process File Count is written successfully => Count : %ld", processItem->getFileCount());

    return true;
}

bool Message::writeProcessFile(const TypeComponentUnit& target,
                               const TypeProcessFile& file, bool isBinary, uint32_t& crc) {

    bool isBinaryTransfer = !file->isOutput() && isBinary;

    if (!writeNumber(target, file->get()->getID(), crc)) {
        LOGS_E(getHost(), "writeProcessFile can not write file ID");
        return false;
    }

    if (!writeNumber(target, file->isOutput(), crc)) {
        LOGS_E(getHost(), "writeProcessFile can not write file state");
        return false;
    }

    if (!writeNumber(target, file->get()->getAssignedJob(), crc)) {
        LOGS_E(getHost(), "writeProcessFile can not write assigned job");
        return false;
    }

    if (!writeNumber(target, file->getAssignedProcess(), crc)) {
        LOGS_E(getHost(), "writeProcessFile can not write assigned job");
        return false;
    }

    if (!writeString(target, file->get()->getName(), crc)) {
        LOGS_E(getHost(), "writeProcessFile can not write file name");
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeString(target, file->get()->getRefPath(), crc)) {
            LOGS_E(getHost(), "writeProcessFile can not write file name");
            return false;
        }

        if (!writeBinary(target, file->get()->getPath(), file->get()->getSize(), crc)) {
            LOGS_E(getHost(), "writeProcessFile can not write file binary");
            return false;
        }

        LOGS_D(getHost(), "Process File Binary is written successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
               file->get()->getID(), file->isOutput()? "output" : "input",
               file->get()->getAssignedJob(), file->getAssignedProcess(), file->get()->getName().c_str());

        return true;
    }

    LOGS_D(getHost(), "Process File Info is written successfully => ID : %ld, State : %s, jobID : %ld, processID : %ld, Name : %s",
           file->get()->getID(), file->isOutput()? "output" : "input",
           file->get()->getAssignedJob(), file->getAssignedProcess(), file->get()->getName().c_str());

    return true;
}

bool Message::writeProcessFiles(const TypeComponentUnit& target,
                                const TypeProcessItem& processItem, bool isBinary, uint32_t& crc) {

    if (!writeProcessID(target, processItem, crc)) {
        return false;
    }

    if (!writeProcessFileCount(target, processItem, crc)) {
        return false;
    }

    for (const auto& processFile : processItem->getFileList()) {

        if (!writeProcessFile(target, processFile, isBinary, crc)) {

            return false;
        }
    }

    return true;
}








bool Message::writeComponentList(const TypeComponentUnit& target, TypeComponentUnitList& componentList, uint32_t& crc) {

    //TODO NOT PROCESSED
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

    //TODO NOT PROCESSED
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




bool Message::writeMessageStream(const TypeComponentUnit& target, uint32_t& crc) {

    switch(getHeader().getStream()) {

        case STREAM_JOBNAME:

            if (!writeJobName(target, data.getJobName(), crc)) {
                return false;
            }

            break;

        case STREAM_FILEBINARY:

            if (!writeFileBinary(target, data.getFile(), crc)) {
                return false;
            }

            break;

        case STREAM_PROCESSID:

            if (!writeProcessID(target, data.getProcess(), crc)) {
                return false;
            }

        case STREAM_PROCESSINFO:

            if (!writeProcessInfo(target, data.getProcess(), crc)) {
                return false;
            }

            break;

        case STREAM_PROCESSFILEINFO:
        case STREAM_PROCESSFILEBINARY:

            if (!writeProcessFile(target, data.getProcessFile(),
                                  getHeader().getStream() == STREAM_PROCESSFILEBINARY, crc)) {
                return false;
            }

            break;

        case STREAM_PROCESSFILEINFO_ALL:
        case STREAM_PROCESSFILEBINARY_ALL:

            if (!writeProcessFiles(target, data.getProcess(),
                                  getHeader().getStream() == STREAM_PROCESSFILEBINARY_ALL, crc)) {
                return false;
            }

            break;







        case STREAM_PROCESS:

//            if (!writeProcess(target, data.getProcess(), crc)) {
//                return false;
//            }
//
//            for (const auto& processFile : data.getProcess()->getFileList()) {
//
//                if (!writeFile(target, processFile, false, crc)) {
//                    return false;
//                }
//            }

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

