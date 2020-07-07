//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#include "ProcessInfo.h"

#include <utility>

ProcessInfo ProcessInfo::invalid = {0};

ProcessInfo::ProcessInfo(int id) :
        id(id), state(PROCESS_STATE_DEPENDENT) {
}

ProcessInfo::ProcessInfo(int id, ProcessItem& item) :
    id(id), item(item), state(PROCESS_STATE_DEPENDENT), assigned(0) {
}

ProcessInfo::ProcessInfo(const ProcessInfo &copy) :
    id(copy.getID()), item(copy.get()), state(copy.getState()) {
}

ProcessItem& ProcessInfo::get() const {

    return (ProcessItem&) item;
}

int ProcessInfo::getID() const {

    return id;
}

void ProcessInfo::setID(int _id) {

    this->id = _id;
}

PROCESS_STATE ProcessInfo::getState() const {

    return state;
}

void ProcessInfo::setState(PROCESS_STATE _state) {

    state = _state;
}

std::string ProcessInfo::getJobID() {

    return jobDir;
}

void ProcessInfo::setJobID(std::string _jobID) {

    jobDir = std::move(_jobID);
}

int ProcessInfo::getAssigned() const {

    return assigned;
}

void ProcessInfo::setAssigned(int _assigned) {

    assigned = _assigned;
}
