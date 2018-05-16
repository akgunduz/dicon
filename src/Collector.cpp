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
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorReadyMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeBinaryMsg);

}

Collector::~Collector() {

}

bool Collector::processDistributorWakeupMsg(ComponentObject owner, Message *msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(ComponentObject owner, Message *msg) {

    setHostID((int)msg->getHeader()->getVariant(0));

    LOG_U(UI_UPDATE_COLL_ID, std::vector<long>{getHost().getID()});
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return true;
}

bool Collector::processDistributorReadyMsg(ComponentObject owner, Message *msg) {

    Job* job = getJobs()->get(msg->getData()->getJobID());
    if (job == NULL) {
        LOGS_I(getHost(), "No available job with this ID : %s", msg->getData()->getJobID().getStr().c_str());
        delete msg;
        return false;
    }

    if (job->getUnServedCount() > 0) {
        return send2DistributorNodeMsg(getDistributor(), job->getJobID(), job->getUnServedCount());
    }

    return true;
}

bool Collector::processDistributorNodeMsg(ComponentObject owner, Message *msg) {

    auto nodeID = (int)msg->getHeader()->getVariant(0);
    long nodeAddress = msg->getHeader()->getVariant(1);

    if (nodeAddress == 0) {

        LOGS_I(getHost(), "No Available Node");
        delete msg;
        return false;
    }

    Job* job = getJobs()->get(msg->getData()->getJobID());
    if (job == NULL) {
        LOGS_I(getHost(), "No available job with this ID : %s", msg->getData()->getJobID().getStr().c_str());
        delete msg;
        return false;
    }

    ExecutorInfo executor = job->getUnServed();

    if (executor.get() == NULL) {
        LOGS_I(getHost(), "No available unServed job right now. So WHY this Node message Come?????");
        delete msg;
        return false;
    }

    LOGS_I(getHost(), "Node[%d] is triggered with Process[%d]", nodeID, executor.getID());

    LOG_U(UI_UPDATE_COLL_PROCESS_LISTITEM, std::vector<long> {executor.getID(), executor.getState(), nodeID});

    return send2NodeJobMsg(NodeObject(nodeID, nodeAddress),
                           job->getJobID(),
                           job->getJobDir(),
                           executor.getID(),
                           executor.get()->getParsedExec(),
                           executor.get()->getFileList());
}

bool Collector::processNodeInfoMsg(ComponentObject owner, Message *msg) {

    LOGS_T(getHost(), "%d File info is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    return send2NodeBinaryMsg(owner,
                              msg->getData()->getJobID(),
                              msg->getData()->getJobDir(),
                              msg->getData()->getFileList());
}

bool Collector::processNodeBinaryMsg(ComponentObject owner, Message *msg) {

    LOGS_T(getHost(), "%d File output binary is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    std::vector<long> fileListIDs;

    for (int i = 0; i < msg->getData()->getFileCount(); i++) {

        fileListIDs.push_back(msg->getData()->getFile(i)->getID());
    }

    LOG_U(UI_UPDATE_COLL_FILE_LISTITEM, fileListIDs);

    Job* job = getJobs()->get(msg->getData()->getJobID());

    job->updateUnServed(msg->getData()->getProcessID(), PROCESS_STATE_ENDED);

    LOG_U(UI_UPDATE_COLL_PROCESS_LISTITEM, std::vector<long> {msg->getData()->getProcessID(), PROCESS_STATE_ENDED, 0});

    LOGS_I(getHost(), "Node[%d]'s output is received for Process[%d]", owner.getID(), msg->getData()->getProcessID());

    return send2NodeReadyMsg(owner);
}

bool Collector::send2DistributorAliveMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Collector::send2DistributorNodeMsg(ComponentObject target, Uuid jobID, long unServedCount) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getHeader()->setVariant(0, unServedCount);

    msg->getData()->setJobID(jobID);

    return send(target, msg);
}

bool Collector::send2NodeJobMsg(ComponentObject target, Uuid jobID, const char* jobDir, long processID,
                                const char* processCommand, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_PROCESS);

    msg->getData()->setJob(jobID, jobDir)
            .setProcess(processID, processCommand)
            .addFileList(fileList, false);

    return send(target, msg);
}

bool Collector::send2NodeBinaryMsg(ComponentObject target, Uuid jobID, const char* jobDir, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setJob(jobID, jobDir)
            .addFileList(fileList, true);

    return send(target, msg);
}

bool Collector::send2NodeReadyMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_READY);

    return send(target, msg);
}

Jobs *Collector::getJobs() {

    return &jobs;
}

ComponentObject Collector::getDistributor() {

    return distributor;
}

void Collector::setDistributor(ComponentObject distributor) {

    this->distributor = distributor;
}

bool Collector::processJob(int index) {

    //TODO Also will add other jobs, after the prev. job is done.

    Job* job = getJobs()->get(index);

    return send2DistributorNodeMsg(getDistributor(), job->getJobID(), job->getUnServedCount());
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
        LOG_U(UI_UPDATE_COLL_PROCESS_LIST, 0, getJobs()->get(0));
    }

    return true;
}
