//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "miniz/miniz.h"
#include "Collector.h"

Collector *Collector::newInstance(const char* path) {

    return new Collector(path);
}

Collector::Collector(const char *rootPath) :
        Component(rootPath), job(nullptr), distributor(getRootPath()) {

    host = new CollectorObject(getRootPath());

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeBinaryMsg);

    initInterfaces(COMP_COLLECTOR);
}

Collector::~Collector() = default;

bool Collector::processDistributorWakeupMsg(const ComponentObject& owner, Message *msg) {

    setDistributor((DistributorObject &) owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(const ComponentObject& owner, Message *msg) {

    setID((int)msg->getHeader()->getVariant(0));

    notifyUI();
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Collector::processDistributorNodeMsg(const ComponentObject& owner, Message *msg) {

    std::vector<ComponentObject>& nodes = msg->getData()->getComponentList();

    LOGS_I(getHost(), "%d assigned node information is came from Distributor", nodes.size());

    if (nodes.empty()) {
        LOGS_I(getHost(), "No available node is came from Distributor", nodes.size());
        delete msg;
        return false;
    }

    for (auto &node : nodes) {
        const ProcessInfo &process = job->assignNode(node);
        LOGS_I(getHost(), "Node[%d] is assigned by distributor, triggering Process[%d]", node.getID(), process.getID());
        send2NodeJobMsg(node, job->getName(), process.getID(),
                        process.get().getParsedProcess(), process.get().getFileList());
    }

    notifyUI();

    return true;
}

bool Collector::processNodeInfoMsg(const ComponentObject& owner, Message *msg) {

    LOGS_T(getHost(), "%d File info is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    return send2NodeBinaryMsg(owner, msg->getData()->getJobName(), msg->getData()->getProcessID(),
                              msg->getData()->getProcess(), msg->getData()->getFileList());
}

bool Collector::processNodeBinaryMsg(const ComponentObject& owner, Message *msg) {

    LOGS_T(getHost(), "%d File output binary is received from Node[%d]", msg->getData()->getFileCount(), owner.getID());

    int totalCount = 0;
    int readyCount = job->updateDependency(msg->getData()->getProcessID(), totalCount);

    notifyUI();

    send2NodeReadyMsg(owner);

    if (readyCount) {

        return send2DistributorNodeMsg(getDistributor(), readyCount);
    }

    if (totalCount == 0) {
        return send2DistributorReadyMsg(getDistributor());
    }

    return true;
}

bool Collector::send2DistributorAliveMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Collector::send2DistributorIDMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    return send(target, msg);
}

bool Collector::send2DistributorNodeMsg(const ComponentObject& target, long readyProcessCount) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getHeader()->setVariant(0, readyProcessCount);

    return send(target, msg);
}

bool Collector::send2DistributorReadyMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_READY);

    return send(target, msg);
}

bool Collector::send2NodeJobMsg(const ComponentObject& target, const char* jobName, int processID,
                                const char* processLine, const TypeFileInfoList &fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_JOB);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobName(jobName);
    msg->getData()->setProcess(processID, processLine);
    msg->getData()->addFileList(fileList);

    return send(target, msg);
}

bool Collector::send2NodeBinaryMsg(const ComponentObject& target, const char* jobName, int processID,
                                   const char* processLine, const TypeFileInfoList &fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobName(jobName);
    msg->getData()->setProcess(processID, processLine);
    msg->getData()->addFileList(fileList);

    return send(target, msg);
}

bool Collector::send2NodeReadyMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_READY);

    return send(target, msg);
}

ComponentObject& Collector::getDistributor() {

    return distributor;
}

void Collector::setDistributor(const DistributorObject& _distributor) {

    this->distributor = _distributor;
}

bool Collector::processJob() {

    int totalCount = 0;
    return send2DistributorNodeMsg(getDistributor(), job->updateDependency(0, totalCount));
}

bool Collector::loadJob(const char* zipFile) {

    delete job;
    job = new Job(getHost(), zipFile, getRootPath());

    if (job->getFileCount() && job->getProcessCount()) {
        notifyUI();
        return true;
    }
    return false;
}

Job *Collector::getJob() {

    return job;
}
