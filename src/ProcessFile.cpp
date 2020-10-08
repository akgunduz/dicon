//
// Created by Haluk AKGUNDUZ on 22.07.2020.
//

#include "ProcessFile.h"

ProcessFile::ProcessFile(TypeFileItem _content, long _processID, PROCESS_FILE_OPTIONS _fileType)
    : processID(_processID), content(std::move(_content)), fileType(_fileType) {
}

long ProcessFile::getAssignedProcess() const {

    return processID;
}

void ProcessFile::setAssignedProcess(long _processID) {

    processID = _processID;
}

TypeFileItem ProcessFile::get() {

    return content;
}

bool ProcessFile::isOutput() const {

    return fileType == PROCESS_FILE_OUTPUT;
}

void ProcessFile::setOutputState(bool _is_output) {

    if (fileType == PROCESS_FILE_OUTPUT && !_is_output) {
        fileType = PROCESS_FILE_INPUT;
        return;
    }

    if (fileType == PROCESS_FILE_INPUT && _is_output) {
        fileType = PROCESS_FILE_OUTPUT;
        return;
    }
}

bool ProcessFile::isExecutable() const {

    return fileType == PROCESS_FILE_EXEC;
}
