//
// Created by akgun on 3.07.2020.
//

#include "NodeProcessInfo.h"

int NodeProcessInfo::getAttachedColl() {

    return attachedCollID;
}

void NodeProcessInfo::setAttachedColl(int id) {

    attachedCollID = id;
}

int NodeProcessInfo::getProcessID() {

    return processID;
}

void NodeProcessInfo::setProcessID(int id) {

    processID = id;
}

std::string NodeProcessInfo::getJobID() {

    return jobDir;
}

void NodeProcessInfo::setJobID(std::string job) {

    jobDir = job;
}
