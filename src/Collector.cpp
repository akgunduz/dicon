//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

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
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileInfoMsg);
    processMsg[COMP_NODE][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileBinaryMsg);

    initInterfaces(COMP_COLLECTOR);

    host->setAddress(COMP_DISTRIBUTOR, getInterfaceAddress(DistributorObject()));
    host->setAddress(COMP_NODE, getInterfaceAddress(NodeObject()));
}

Collector::~Collector() = default;

bool Collector::processDistributorWakeupMsg(const ComponentObject& owner, Message *msg) {

    setDistributor((DistributorObject &) owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(const ComponentObject& owner, Message *msg) {

    setID(msg->getHeader().getVariant(0));

    notifyUI();
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Collector::processDistributorNodeMsg(const ComponentObject& owner, Message *msg) {

    std::vector<ComponentObject>& nodes = msg->getData().getComponentList();

    LOGS_I(getHost(), "Distributor assigned %d node", nodes.size());

    if (nodes.empty()) {
        LOGS_I(getHost(), "No available node is came from Distributor", nodes.size());
        delete msg;
        return false;
    }

    for (auto &node : nodes) {
        ProcessItem *process = job->assignNode(node);
        LOGS_I(getHost(), "Node[%d]:Process[%d] is triggered ", node.getID(), process->getID());
        send2NodeProcessMsg(node, process);
    }

    notifyUI();

    return true;
}

bool Collector::processNodeFileInfoMsg(const ComponentObject& owner, Message *msg) {

    LOGS_T(getHost(), "Node[%d]:Process[%d] requested %d missing files",
            owner.getID(), msg->getData().getFileProcess(), msg->getData().getFileCount());

    auto *processItem = job->getProcessByID(msg->getData().getFileProcess());
    if (!processItem) {

        LOGS_E(getHost(), "Process[%d] could not find in the Job!", msg->getData().getFileProcess());
        return false;
    }

    TypeProcessFileList newList;

    for (auto file : msg->getData().getFileList()) {

        newList.emplace_back(processItem->getFile(file));
    }

    return send2NodeBinaryMsg(owner, processItem->getID(), newList);
}

bool Collector::processNodeFileBinaryMsg(const ComponentObject& owner, Message *msg) {

    LOGS_T(getHost(), "Node[%d] sent %d File output binaries", owner.getID(), msg->getData().getFileCount());

    int totalCount = 0;
    int readyCount = job->updateDependency(msg->getData().getFileProcess(), totalCount);

    notifyUI();

    send2NodeReadyMsg(owner);

    if (readyCount) {

        return send2DistributorNodeMsg(distributor, readyCount);
    }

    if (totalCount == 0) {

        return send2DistributorReadyMsg(distributor);
    }

    return true;
}

bool Collector::send2DistributorAliveMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Collector::send2DistributorIDMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_ID);

    return send(target, msg);
}

bool Collector::send2DistributorNodeMsg(const ComponentObject& target, long readyProcessCount) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_NODE);

    msg->getHeader().setVariant(0, readyProcessCount);

    return send(target, msg);
}

bool Collector::send2DistributorReadyMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_READY);

    return send(target, msg);
}

bool Collector::send2NodeProcessMsg(const ComponentObject& target, ProcessItem *processItem) {

    auto *msg = new Message(getHost(), COMP_NODE, MSGTYPE_PROCESS);

    msg->getData().setStreamFlag(STREAM_PROCESS);
    msg->getData().addProcess(processItem);

    return send(target, msg);
}

bool Collector::send2NodeBinaryMsg(const ComponentObject& target, long processID, TypeProcessFileList &fileList) {

    auto *msg = new Message(getHost(), COMP_NODE, MSGTYPE_BINARY);

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(processID, fileList);

    return send(target, msg);
}

bool Collector::send2NodeReadyMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_NODE, MSGTYPE_READY);

    return send(target, msg);
}

void Collector::setDistributor(const DistributorObject& _distributor) {

    distributor = _distributor;
}

bool Collector::processJob() {

    int totalCount = 0;
    return send2DistributorNodeMsg(distributor, job->updateDependency(0, totalCount));
}

bool Collector::loadJob(const char* zipFile) {

    delete job;

    job = new JobItem(getHost(), zipFile, JobItem::jobID++);

    notifyUI();

    return true;
}

JobItem *Collector::getJob() {

    return job;
}
