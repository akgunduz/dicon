//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "NodeObject.h"

Collector *Collector::newInstance(const char* path) {

    return new Collector(path);
}

Collector::Collector(const char *rootPath) :
        Component(COMP_COLLECTOR, rootPath), distributor(COMP_DISTRIBUTOR, getRootPath()){

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeBinaryMsg);

    setDistributorAddress(0);
}

Collector::~Collector() {

}

bool Collector::processDistributorWakeupMsg(ComponentObject owner, long address, Message *msg) {

    setDistributorAddress(address);

    return send2DistributorAliveMsg(owner, address);
}

bool Collector::processDistributorIDMsg(ComponentObject owner, long address, Message *msg) {

    setHostID((int)msg->getHeader()->getVariant(0));

    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return true;
}

bool Collector::processDistributorNodeMsg(ComponentObject owner, long address, Message *msg) {

    int id = msg->getHeader()->getVariant(0);
    long nodeAddress = msg->getHeader()->getVariant(1);

    if (nodeAddress == 0) {

        LOGS_I(getHost(), "No Available Node");
        delete msg;
        return false;
    }

    if (strcmp(msg->getData()->getJobDir(), "") == 0 ||
            !Util::checkPath(getRootPath(), msg->getData()->getJobDir(), false)) {
        LOGS_I(getHost(), "No Job at path : \"%s\" is found!!!", msg->getData()->getJobDir());
        delete msg;
        return false;
    }

    Job* job = getJobs()->get(msg->getData()->getJobDir());

    ExecutorInfo executor = job->getUnServed();

    if (executor.get() == NULL) {
        LOGS_I(getHost(), "No available unServed job right now. So WHY this Node message Come?????");
        delete msg;
        return false;
    }

    LOGS_I(getHost(), "Process[%d] is triggered at Node[%d]", executor.getID(), id);

    LOG_U(UI_UPDATE_COLL_PROCESS_LIST, job);

    return send2NodeJobMsg(NodeObject(id), nodeAddress, msg->getData()->getJobDir(), executor.getID(),
                           executor.get()->getParsedExec(), executor.get()->getFileList());
}

bool Collector::processNodeInfoMsg(ComponentObject owner, long address, Message *msg) {

    LOGS_I(getHost(), "%d File info is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    return send2NodeBinaryMsg(owner, address, msg->getData()->getJobDir(), msg->getData()->getExecutorID(),
                              msg->getData()->getExecutor(), msg->getData()->getFileList());
}

bool Collector::processNodeBinaryMsg(ComponentObject owner, long address, Message *msg) {

    LOGS_I(getHost(), "%d File output binary is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    std::vector<long> fileListIDs;

    for (int i = 0; i < msg->getData()->getFileCount(); i++) {

        fileListIDs.push_back(msg->getData()->getFile(i)->getID());
    }

    LOG_U(UI_UPDATE_COLL_FILE_LISTITEM, fileListIDs);

    Job* job = getJobs()->get(msg->getData()->getJobDir());

    job->setOrderedState(msg->getData()->getExecutorID(), PROCESS_STATE_ENDED);

    LOG_U(UI_UPDATE_COLL_PROCESS_LIST, job);

    TypeMD5List md5List;

    bool status = send2NodeReadyMsg(owner, address, msg->getData()->getJobDir());

    if (getJobs()->get(msg->getData()->getJobDir())->getUnServedCount() > 0) {

        status &= send2DistributorNodeMsg(getDistributor(),
                                          getDistributorAddress(), msg->getData()->getJobDir(), &md5List);
    }

    return status;
}

bool Collector::send2DistributorAliveMsg(ComponentObject target, long address) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, address, msg);
}

bool Collector::send2DistributorNodeMsg(ComponentObject target, long address, const char* jobDir, TypeMD5List *md5List) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_MD5);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->addMD5List(md5List);

    return send(target, address, msg);
}

bool Collector::send2NodeJobMsg(ComponentObject target, long address, const char* jobDir, long executionID,
                                const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_JOB);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executionID, executor);
    msg->getData()->addFileList(fileList);

    return send(target, address, msg);
}

bool Collector::send2NodeBinaryMsg(ComponentObject target, long address, const char* jobDir, long executionID,
                                   const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executionID, executor);
    msg->getData()->addFileList(fileList);

    return send(target, address, msg);
}

bool Collector::send2NodeReadyMsg(ComponentObject target, long address, const char* jobDir) {

    auto *msg = new Message(getHost(), MSGTYPE_READY);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getData()->setJobDir(jobDir);

    return send(target, address, msg);
}

Jobs *Collector::getJobs() {

    return &jobs;
}

ComponentObject Collector::getDistributor() {

    return distributor;
}

long Collector::getDistributorAddress() {

    return distributorAddress;
}

void Collector::setDistributorAddress(long address) {

    distributorAddress = address;
}

bool Collector::processJob(int index) {

    //TODO Whole executors will be replaced with only independent executors
    //TODO Also will add other jobs, after the prev. job is done.

//    for (int k = 0; k < 1; k++) {
    for (int k = 0; k < getJobs()->get(index)->getUnServedCount(); k++) {

        TypeMD5List md5List;
        send2DistributorNodeMsg(getDistributor(), getDistributorAddress(),
                                getJobs()->get(index)->getJobDir(), &md5List);
    }

    return true;
}

bool Collector::processJobs() {

    for (int i = 0; i < getJobs()->getCount(); i++) {

        processJob(i);
    }

    return true;
}

bool Collector::loadJob(const char* path) {

    getJobs()->addPath(getHost(), true);

    if (getJobs()->isEmpty()) {
        return false;
    }

    if (getJobs()->get(0)->getFileCount()) {
        LOG_U(UI_UPDATE_COLL_FILE_LIST, getJobs()->get(0));
    }

    if (getJobs()->get(0)->getExecutorCount()) {
        LOG_U(UI_UPDATE_COLL_PROCESS_LIST, getJobs()->get(0));
    }

    return true;
}
