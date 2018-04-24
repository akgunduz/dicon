//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector::Collector(const char *rootPath) :
        Component(COMP_COLLECTOR, rootPath){

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeBinaryMsg);

	LOG_U(UI_UPDATE_COLL_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addList(getHost(), ComponentTypes::getRootPath(COMP_COLLECTOR), true);

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

bool Collector::processDistributorNodeMsg(long address, Message *msg) {

    long nodeAddress = msg->getHeader()->getVariant(0);

    if (nodeAddress == 0) {

        LOG_U(UI_UPDATE_COLL_LOG, "No Available Node");
        delete msg;
        return false;
    }

    //TODO
    ExecutorItem* executor = getJobs()->get(0)->getUnServed();

    if (executor == NULL) {
        LOG_W("No available unServed job right now.");
        delete msg;
        return false;
    }

    LOG_T("New Job created from path : %s", "TODO JOB");

    LOG_U(UI_UPDATE_COLL_LOG, "Available Node : %s",
          InterfaceTypes::getAddressString(nodeAddress).c_str());

    TypeFileInfoList list = FileInfo::getFileList(executor->getFileList(), FILEINFO_ALL);

    return send2NodeJobMsg(nodeAddress, getJobs()->get(0)->getJobDir(),
                           executor->getParsedExec(), &list);
}

bool Collector::processNodeInfoMsg(long address, Message *msg) {

    LOG_U(UI_UPDATE_COLL_LOG, "%d File info received", msg->getData()->getFileCount());

    return send2NodeBinaryMsg(address, msg->getData()->getJobDir(),
                              msg->getData()->getExecutor(), msg->getData()->getFileList());
}

bool Collector::processNodeBinaryMsg(long address, Message *msg) {

    //TODO next step integrate node output to dependancy list

    LOG_U(UI_UPDATE_COLL_LOG, "%d File output binary received", msg->getData()->getFileCount());

    getJobs()->get(0)->updateIndependentExecutions(msg->getData()->getFileList());

    TypeMD5List md5List;

    bool status = send2NodeReadyMsg(address);

    return status && send2DistributorNodeMsg(address, &md5List);
}

bool Collector::send2DistributorAliveMsg(long address) {

    auto *msg = new Message(COMP_COLLECTOR, MSGTYPE_ALIVE);

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Collector::send2DistributorNodeMsg(long address, TypeMD5List *md5List) {

    auto *msg = new Message(COMP_COLLECTOR, MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_MD5);
    msg->getData()->addMD5List(md5List);

    LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" necessary file md5s are prepared", md5List->size());

    return send(COMP_DISTRIBUTOR, address, msg);
}

bool Collector::send2NodeJobMsg(long address, const char* jobDir, const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(COMP_COLLECTOR, MSGTYPE_JOB);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executor);
    msg->getData()->addFileList(fileList);

    LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file info is prepared for execution : %s", fileList->size(), executor);

    return send(COMP_NODE, address, msg);
}

bool Collector::send2NodeBinaryMsg(long address, const char* jobDir, const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(COMP_COLLECTOR, MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executor);
    msg->getData()->addFileList(fileList);

    LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file binary is prepared", fileList->size());

    return send(COMP_NODE, address, msg);
}

bool Collector::send2NodeReadyMsg(long address) {

    auto *msg = new Message(COMP_COLLECTOR, MSGTYPE_READY);

    return send(COMP_NODE, address, msg);
}

bool Collector::processJob() {

    //TODO Whole executors will be replaced with only independent executors
    //TODO Also will add other jobs, after the prev. job is done.


 //   for (int k = 0; k < 1; k++) {
    for (int k = 0; k < jobs.get(0)->getUnServedCount(); k++) {
        TypeMD5List md5List;
//        auto *executorItem = jobs.get(0)->getOrdered(k);
//        for (int i = 0; i < executorItem->getFileCount(); i++) {
//            md5List.push_back(*executorItem->getFile(i)->getMD5());
//        }

        send2DistributorNodeMsg(distributorAddress, &md5List);
    }

    return true;
}

Jobs *Collector::getJobs() {

    return &jobs;
}

bool Collector::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}
