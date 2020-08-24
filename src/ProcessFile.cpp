//
// Created by Haluk AKGUNDUZ on 22.07.2020.
//

#include "ProcessFile.h"

ProcessFile::ProcessFile(TypeFileItem _content, long _processID, bool _is_output)
    : processID(_processID), content(_content), is_output(_is_output) {
}

bool ProcessFile::isOutput() const {

    return is_output;
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

void ProcessFile::setOutputState(bool _is_output) {

    is_output = _is_output;
}
