//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "NodeObject.h"
#include "CollectorObject.h"

Collector *Collector::newInstance(const char* path) {

    return new Collector(path);
}

Collector::Collector(const char *rootPath) :
        Component(rootPath), distributor(getRootPath()) {

    host = new CollectorObject(getRootPath());

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeBinaryMsg);

    initInterfaces(COMP_COLLECTOR);
}

Collector::~Collector() {

}

bool Collector::processDistributorWakeupMsg(ComponentObject owner, Message *msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(ComponentObject owner, Message *msg) {

    getHost().setID((int)msg->getHeader()->getVariant(0));

    LOG_U(UI_UPDATE_COLL_ID, std::vector<long>{getHost().getID()});
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Collector::processDistributorNodeMsg(ComponentObject owner, Message *msg) {

    std::vector<ComponentObject>& nodes = msg->getData()->getComponentList();

    LOGS_I(getHost(), "%d assigned node information is came from Distributor", nodes.size());

    if (nodes.size() == 0) {

        delete msg;
        return false;
    }

    job->updateDependency();

    if (nodes.size() > job->getProcessCount(PROCESS_STATE_READY)) {

        LOGS_I(getHost(), "%d assigned node is bigger than the request, WHY!!!", nodes.size());
        assert(true);
        delete msg;
        return false;
    }

    std::vector<ProcessInfo> readyProcesses;


  //  ProcessInfo &executor = job->getUnServed();




//    if (strcmp(msg->getData()->getJobDir(), "") == 0 ||
//            !Util::checkPath(getRootPath(), msg->getData()->getJobDir(), false)) {
//        LOGS_I(getHost(), "No Job at path : \"%s\" is found!!!", msg->getData()->getJobDir());
//        delete msg;
//        return false;
//    }
//
//    Job* job = getJobs()->get(msg->getData()->getJobDir());
//
//    ProcessInfo &executor = job->getUnServed();
//
//    if (executor.get() == nullptr) {
//        LOGS_I(getHost(), "Node[%d] is assigned by distributor, but NO available unServed job right now. So WHY this Node message Come?????");
//        delete msg;
//        return false;
//    }
//
//    executor.setAssignedNode(nodeObj.getID());
//
//    LOGS_I(getHost(), "Node[%d] is assigned by distributor, triggering Process[%d]", nodeObj.getID(), executor.getID());
//
//    LOG_U(UI_UPDATE_COLL_PROCESS_LISTITEM, job);
//
//    return send2NodeJobMsg(nodeObj, msg->getData()->getJobDir(), executor.getID(),
//                           executor.get()->getParsedExec(), executor.get()->getFileList());
}

bool Collector::processNodeInfoMsg(ComponentObject owner, Message *msg) {

    LOGS_T(getHost(), "%d File info is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    return send2NodeBinaryMsg(owner, msg->getData()->getJobDir(), msg->getData()->getExecutorID(),
                              msg->getData()->getExecutor(), msg->getData()->getFileList());
}

bool Collector::processNodeBinaryMsg(ComponentObject owner, Message *msg) {

    LOGS_T(getHost(), "%d File output binary is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());
//
//    std::vector<long> fileListIDs;
//
//    for (int i = 0; i < msg->getData()->getFileCount(); i++) {
//
//        fileListIDs.push_back(msg->getData()->getFile(i)->getID());
//    }
//
//    LOG_U(UI_UPDATE_COLL_FILE_LISTITEM, fileListIDs);
//
//    getJob()->setOrderedState(msg->getData()->getExecutorID(), PROCESS_STATE_ENDED);
//
//    LOG_U(UI_UPDATE_COLL_PROCESS_LISTITEM, getJob());
//
//    TypeMD5List md5List;
//
//    return send2NodeReadyMsg(owner, msg->getData()->getJobDir(), getJob()->getReadyCount());
}

bool Collector::send2DistributorAliveMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Collector::send2DistributorIDMsg(ComponentObject target) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    return send(target, msg);
}

bool Collector::send2DistributorNodeMsg(ComponentObject target, long readyProcessCount) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getHeader()->setVariant(0, readyProcessCount);

    return send(target, msg);
}

bool Collector::send2NodeJobMsg(ComponentObject target, const char* jobDir, long executionID,
                                const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_JOB);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executionID, executor);
    msg->getData()->addFileList(fileList);

    return send(target, msg);
}

bool Collector::send2NodeBinaryMsg(ComponentObject target, const char* jobDir, long executionID,
                                   const char* executor, TypeFileInfoList *fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executionID, executor);
    msg->getData()->addFileList(fileList);

    return send(target, msg);
}

bool Collector::send2NodeReadyMsg(ComponentObject target, const char* jobDir, long collUnservedCount) {

    auto *msg = new Message(getHost(), MSGTYPE_READY);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getData()->setJobDir(jobDir);

    msg->getHeader()->setVariant(0, collUnservedCount);

    return send(target, msg);
}

ComponentObject Collector::getDistributor() {

    return distributor;
}

void Collector::setDistributor(const DistributorObject& _distributor) {

    this->distributor = _distributor;
}

bool Collector::processJob() {

    //TODO Whole executors will be replaced with only independent executors
    //TODO Also will add other jobs, after the prev. job is done.

    getJob()->updateDependency();
    return send2DistributorNodeMsg(getDistributor(), getJob()->getProcessCount(PROCESS_STATE_READY));
}

bool Collector::loadJob(const char* path) {

    std::vector<std::string> dirList = Util::getDirList(getHost().getRootPath(), JOB_DIR_PREFIX);

    job = new Job(getHost(), dirList[0].c_str());

    if (job->getFileCount() && job->getExecutorCount()) {
        LOG_U(UI_UPDATE_COLL_FILE_LIST, job);
    }
}

Job *Collector::getJob() {

    return job;
}
