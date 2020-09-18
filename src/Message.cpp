//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Message.h"
#include "ComponentUnitFactory.h"

Message::Message(const TypeHostUnit& host)
		: MessageBase(host), data(getHost()) {
}

Message::Message(const TypeHostUnit& host, COMPONENT targetType, MSG_TYPE msgType, STREAM_TYPE streamType)
		: MessageBase(host, targetType, msgType, streamType), data(getHost()) {
}

bool Message::readJobName(const TypeComponentUnit& source, std::string& jobName) {

    jobName = strings.front();
    strings.pop_front();

    LOGS_D(getHost(), "Job Name is read successfully => Name : %s", jobName.c_str());

    return true;
}

bool Message::readID(const TypeComponentUnit& source, TypeID& id) {

    id = (TypeID)numbers.front();
    numbers.pop_front();

    LOGS_D(getHost(), "ID[%ld] is read successfully", id);

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

    processItem->setID((TypeID)numbers.front());
    numbers.pop_front();

    LOGS_D(getHost(), "Process[%ld] is read successfully", processItem->getID());

    return true;
}

bool Message::readProcessCount(const TypeComponentUnit& source, uint32_t& processCount) {

    processCount = (uint32_t)numbers.front();
    numbers.pop_front();

    LOGS_D(getHost(), "Process Count is read successfully => %d", processCount);

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

    count = (long) numbers.front();
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

bool Message::readProcess(const TypeComponentUnit& source, const TypeProcessItem& processItem) {

    readProcessInfo(source, processItem);

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

bool Message::readComponentList(const TypeComponentUnit& source, TypeComponentUnitList& componentList) {

    auto type = (COMPONENT) numbers.front();
    numbers.pop_front();

    long componentCount = (long) numbers.front();
    numbers.pop_front();

    for (long i = 0; i < componentCount; i++) {

        auto compUnit = std::make_shared<ComponentUnit>(type);

        compUnit->setArch((ARCH)numbers.front());
        numbers.pop_front();

        compUnit->setID((TypeID) numbers.front());
        numbers.pop_front();

        uint32_t base = (uint32_t) numbers.front();
        numbers.pop_front();

        uint16_t port = (uint16_t) numbers.front();
        numbers.pop_front();

        //TODO comm interface type
        compUnit->getAddress()->set(COMMINTERFACE_TCPIP, base, port);

        base = (uint32_t) numbers.front();
        numbers.pop_front();

        port = (uint16_t) numbers.front();
        numbers.pop_front();

        compUnit->getAddress()->setUI(base, port);

        componentList.emplace_back(ComponentUnitFactory::create(type, compUnit->getArch(),
                                                                compUnit->getID(), compUnit->getAddress()));

    }

    LOGS_D(getHost(), "Component list is read successfully => Count : %d", componentList.size());
    for (auto &item : componentList) {
        LOGS_D(getHost(), "Component[%s] : %d", ComponentType::getName(item->getType()), item->getID());
    }

    return true;
}

bool Message::build(const TypeComponentUnit& source) {

    switch(getHeader().getStream()) {

        case STREAM_ID:

            readID(source, data.getID());

            break;

        case STREAM_JOBNAME:

            readJobName(source, data.getJobName());

            break;

        case STREAM_FILE_BINARY:

            readFileBinary(source, data.getFile());

            break;

        case STREAM_PROCESS_ID:

            readProcessID(source, data.getProcess());

            break;

        case STREAM_PROCESS_COUNT:

            readProcessCount(source, data.getProcessCount());

            break;

        case STREAM_PROCESS_INFO:

            readProcessInfo(source, data.getProcess());

            break;

        case STREAM_PROCESS_FILE_INFO:
        case STREAM_PROCESS_FILE_BINARY:

            readProcessFile(source, data.getProcessFile());

            break;

        case STREAM_PROCESS_FILES_INFO:
        case STREAM_PROCESS_FILES_BINARY:

            readProcessFiles(source, data.getProcess());

            break;

        case STREAM_PROCESS:

            readProcess(source, data.getProcess());

            break;

        case STREAM_COMPONENTS:

            readComponentList(source, data.getComponentList());

            break;

        default:
            break;
    }

    return true;
}

bool Message::writeID(const TypeComponentUnit& target, const TypeID& id) {

    if (!writeNumber(target, id)) {
        LOGS_E(getHost(), "writeID can not write job info");
        return false;
    }

    LOGS_D(getHost(), "ID is written successfully => %d", id);

    return true;
}

bool Message::writeJobName(const TypeComponentUnit& target, const std::string& jobName) {

    if (!writeString(target, jobName)) {
        LOGS_E(getHost(), "writeJobName can not write job info");
        return false;
    }

    LOGS_D(getHost(), "Job Name is written successfully => Name : %s", jobName.c_str());

    return true;
}

bool Message::writeFileBinary(const TypeComponentUnit& target, const TypeFileItem& file) {

    if (!writeString(target, file->getName())) {
        LOGS_E(getHost(), "writeFileBinary can not write file name");
        return false;
    }

    if (!writeBinary(target, file)) {
        LOGS_E(getHost(), "writeFileBinary can not write file binary");
        return false;
    }

    LOGS_D(getHost(), "File Binary is written successfully => Name : %s", file->getName().c_str());

    return true;
}

bool Message::writeProcessID(const TypeComponentUnit& target, const TypeProcessItem& processItem) {

    if (!writeNumber(target, processItem->getID())) {
        LOGS_E(getHost(), "writeProcessID can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process ID is written successfully => %ld", processItem->getID());

    return true;
}

bool Message::writeProcessCount(const TypeComponentUnit& target, const uint32_t& processCount) {

    if (!writeNumber(target, processCount)) {
        LOGS_E(getHost(), "writeProcessCount can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process Count is written successfully => %ld", processCount);

    return true;
}

bool Message::writeProcessInfo(const TypeComponentUnit& target, const TypeProcessItem& processItem) {

    if (!writeNumber(target, processItem->getID())) {
        LOGS_E(getHost(), "writeProcessInfo can not write process ID");
        return false;
    }

    if (!writeNumber(target, processItem->getAssignedJob())) {
        LOGS_E(getHost(), "writeProcessInfo can not write job ID");
        return false;
    }

    if (!writeString(target, processItem->getParsedProcess())) {
        LOGS_E(getHost(), "writeProcessInfo can not write process info");
        return false;
    }

    LOGS_D(getHost(), "Process Info is written successfully => ID : %ld, jobID : %ld, Process : %s",
           processItem->getID(), processItem->getAssignedJob(), processItem->getParsedProcess().c_str());

    return true;
}

bool Message::writeProcessFileCount(const TypeComponentUnit& target, const TypeProcessItem& processItem) {

    if (!writeNumber(target, processItem->getFileCount())) {
        LOGS_E(getHost(), "writeProcessFileCount can not write process ID");
        return false;
    }

    LOGS_D(getHost(), "Process File Count is written successfully => Count : %ld", processItem->getFileCount());

    return true;
}

bool Message::writeProcessFile(const TypeComponentUnit& target,
                               const TypeProcessFile& file, bool isBinary) {

    bool isBinaryTransfer = !file->isOutput() && isBinary;

    if (!writeNumber(target, file->get()->getID())) {
        LOGS_E(getHost(), "writeProcessFile can not write file ID");
        return false;
    }

    if (!writeNumber(target, file->isOutput())) {
        LOGS_E(getHost(), "writeProcessFile can not write file state");
        return false;
    }

    if (!writeNumber(target, file->get()->getAssignedJob())) {
        LOGS_E(getHost(), "writeProcessFile can not write assigned job");
        return false;
    }

    if (!writeNumber(target, file->getAssignedProcess())) {
        LOGS_E(getHost(), "writeProcessFile can not write assigned job");
        return false;
    }

    if (!writeString(target, file->get()->getName())) {
        LOGS_E(getHost(), "writeProcessFile can not write file name");
        return false;
    }

    if (isBinaryTransfer) {

        if (!writeBinary(target, file->get())) {
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
                                const TypeProcessItem& processItem, bool isBinary) {

    if (!writeProcessID(target, processItem)) {
        return false;
    }

    if (!writeProcessFileCount(target, processItem)) {
        return false;
    }

    for (const auto& processFile : processItem->getFileList()) {

        if (!writeProcessFile(target, processFile, isBinary)) {

            return false;
        }
    }

    return true;
}

bool Message::writeProcess(const TypeComponentUnit& target, const TypeProcessItem& processItem) {

    if (!writeProcessInfo(target, processItem)) {
        return false;
    }

    if (!writeProcessFileCount(target, processItem)) {
        return false;
    }

    for (const auto& processFile : processItem->getFileList()) {

        if (!writeProcessFile(target, processFile, false)) {

            return false;
        }
    }

    return true;
}

bool Message::writeComponentList(const TypeComponentUnit& target, TypeComponentUnitList& componentList) {

    if (componentList.empty()) {
        LOGS_E(getHost(), "writeComponentList list is empty");
        return false;
    }

    if (!writeNumber(target, componentList[0]->getType())) {
        LOGS_E(getHost(), "writeComponentList can not write component type");
        return false;
    }

    if (!writeNumber(target, componentList.size())) {
        LOGS_E(getHost(), "writeComponentList can not write component count");
        return false;
    }

    for (auto &component : componentList) {

        if (!writeNumber(target, component->getArch())) {
            LOGS_E(getHost(), "writeComponentList can not write component item");
            return false;
        }

        if (!writeNumber(target, component->getID())) {
            LOGS_E(getHost(), "writeComponentList can not write component item");
            return false;
        }

        if (!writeNumber(target, component->getAddress()->get().base)) {
            LOGS_E(getHost(), "writeComponentList can not write component item");
            return false;
        }

        if (!writeNumber(target, component->getAddress()->get().port)) {
            LOGS_E(getHost(), "writeComponentList can not write component item");
            return false;
        }

        if (!writeNumber(target, component->getAddress()->getUI().base)) {
            LOGS_E(getHost(), "writeComponentList can not write component item");
            return false;
        }

        if (!writeNumber(target, component->getAddress()->getUI().port)) {
            LOGS_E(getHost(), "writeComponentList can not write component item");
            return false;
        }

    }

    LOGS_D(getHost(), "Component list is written successfully => Count : %d", componentList.size());

    for (auto& item : componentList) {

        LOGS_D(getHost(), "Component[%s] : %d", ComponentType::getName(item->getType()), item->getID());

    }

    return true;
}

bool Message::writeMessageStream(const TypeComponentUnit& target) {

    switch(getHeader().getStream()) {

        case STREAM_ID:

            if (!writeID(target, data.getID())) {
                return false;
            }

            break;

        case STREAM_JOBNAME:

            if (!writeJobName(target, data.getJobName())) {
                return false;
            }

            break;

        case STREAM_FILE_BINARY:

            if (!writeFileBinary(target, data.getFile())) {
                return false;
            }

            break;

        case STREAM_PROCESS_ID:

            if (!writeProcessID(target, data.getProcess())) {
                return false;
            }

            break;

        case STREAM_PROCESS_COUNT:

            if (!writeProcessCount(target, data.getProcessCount())) {
                return false;
            }

            break;

        case STREAM_PROCESS_INFO:

            if (!writeProcessInfo(target, data.getProcess())) {
                return false;
            }

            break;

        case STREAM_PROCESS_FILE_INFO:
        case STREAM_PROCESS_FILE_BINARY:

            if (!writeProcessFile(target, data.getProcessFile(),
                                  getHeader().getStream() == STREAM_PROCESS_FILE_BINARY)) {
                return false;
            }

            break;

        case STREAM_PROCESS_FILES_INFO:
        case STREAM_PROCESS_FILES_BINARY:

            if (!writeProcessFiles(target, data.getProcess(),
                                  getHeader().getStream() == STREAM_PROCESS_FILES_BINARY)) {
                return false;
            }

            break;

        case STREAM_PROCESS:

            if (!writeProcess(target, data.getProcess())) {
                return false;
            }

            break;

        case STREAM_COMPONENTS: {

            if (!writeComponentList(target, data.getComponentList())) {
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

