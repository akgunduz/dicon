//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector *Collector::newInstance(int id) {

    char path[PATH_MAX];
    sprintf(path, "%s/%s_%d", getcwd(NULL, 0), COLLECTOR_PATH, id);
    mkdir(path, 0777);

    auto *instance = new Collector(path);

    return instance;
}

Collector::Collector(const char *rootPath) :
        Component(COMP_COLLECTOR, rootPath){

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeBinaryMsg);

	LOG_U(UI_UPDATE_COLL_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addPath(getHost(), ComponentTypes::getRootPath(COMP_COLLECTOR), true);

    if (getJobs()->isEmpty()) {
        return;
    }

    LOG_U(UI_UPDATE_COLL_FILE_LIST, getJobs()->get(0));
	LOG_U(UI_UPDATE_COLL_PROCESS_LIST, getJobs()->get(0));
}

Collector::~Collector() {

}

bool Collector::processDistributorWakeupMsg(long address, Message *msg) {

    setDistributorAddress(address);

    LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

    return send2DistributorAliveMsg(address);
}

bool Collector::processDistributorIDMsg(long address, Message *msg) {

    setID((int)msg->getHeader()->getVariant(0));

    LOGS_I(getHost(), getID(), "New ID : %d is assigned by Distributor", getID());

    return true;
}

bool Collector::processDistributorNodeMsg(long address, Message *msg) {

    long nodeAddress = msg->getHeader()->getVariant(0);

    if (nodeAddress == 0) {

        LOGS_I(getHost(), getID(), "No Available Node");
        delete msg;
        return false;
    }

    if (strcmp(msg->getData()->getJobDir(), "") == 0 || !Util::checkPath(ComponentTypes::getRootPath(getHost()), msg->getData()->getJobDir(), false)) {
        LOGS_I(getHost(), getID(), "No Job at path : \"%s\" is found!!!", msg->getData()->getJobDir());
        delete msg;
        return false;
    }

    ExecutorItem* executor = getJobs()->get(msg->getData()->getJobDir())->getUnServed();

    if (executor == NULL) {
        LOGS_I(getHost(), getID(), "No available unServed job right now. So WHY this Node message Come?????");
        delete msg;
        return false;
    }

    LOGS_I(getHost(), getID(), "Job execution at path : %s has started", msg->getData()->getJobDir());

    LOGS_I(getHost(), getID(), "Available Node : %s",
          InterfaceTypes::getAddressString(nodeAddress).c_str());

    return send2NodeJobMsg(nodeAddress, msg->getData()->getJobDir(),
                           executor->getParsedExec(), executor->getFileList());
}

bool Collector::processNodeInfoMsg(long address, Message *msg) {

    LOGS_I(getHost(), getID(), "%d File info received", msg->getData()->getFileCount());

    return send2NodeBinaryMsg(address, msg->getData()->getJobDir(),
                              msg->getData()->getExecutor(), msg->getData()->getFileList());
}

bool Collector::processNodeBinaryMsg(long address, Message *msg) {

    LOGS_I(getHost(), getID(), "%d File output binary received", msg->getData()->getFileCount());

    LOG_U(UI_UPDATE_COLL_FILE_LISTITEM, getJobs()->get(0));

    TypeMD5List md5List;

    bool status = send2NodeReadyMsg(address);

    if (getJobs()->get(msg->getData()->getJobDir())->getUnServedCount() > 0) {

        status &= send2DistributorNodeMsg(getDistributorAddress(), msg->getData()->getJobDir(), &md5List);
    }

    return status;
}

bool Collector::send2DistributorAliveMsg(long address) {

    auto *msg = new Message(getHost(), getID(), MSGTYPE_ALIVE);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Collector::send2DistributorNodeMsg(long address, const char* jobDir, TypeMD5List *md5List) {

    auto *msg = new Message(getHost(), getID(), MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_MD5);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->addMD5List(md5List);

    LOGS_I(getHost(), getID(), "\"%d\" necessary file md5s are prepared", md5List->size());

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Collector::send2NodeJobMsg(long address, const char* jobDir, const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), getID(), MSGTYPE_JOB);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executor);
    msg->getData()->addFileList(fileList);

    LOGS_I(getHost(), getID(), "\"%d\" file info is prepared for execution : %s", fileList->size(), executor);

    return send(COMP_NODE, address, msg);
}

bool Collector::send2NodeBinaryMsg(long address, const char* jobDir, const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), getID(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executor);
    msg->getData()->addFileList(fileList);

    LOGS_I(getHost(), getID(), "\"%d\" file binary is prepared", fileList->size());

    return send(COMP_NODE, address, msg);
}

bool Collector::send2NodeReadyMsg(long address) {

    auto *msg = new Message(getHost(), getID(), MSGTYPE_READY);

    return send(COMP_NODE, address, msg);
}

Jobs *Collector::getJobs() {

    return &jobs;
}

long Collector::getDistributorAddress() {

    return distributorAddress;
}

bool Collector::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}

bool Collector::processJob(int index) {

    //TODO Whole executors will be replaced with only independent executors
    //TODO Also will add other jobs, after the prev. job is done.

//    for (int k = 0; k < 1; k++) {
    for (int k = 0; k < getJobs()->get(index)->getUnServedCount(); k++) {

        TypeMD5List md5List;
        send2DistributorNodeMsg(getDistributorAddress(),
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
