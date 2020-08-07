//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "CollectorHost.h"

Collector *Collector::newInstance(const char* path, int interfaceOther, int interfaceNode) {

    return new Collector(path, interfaceOther, interfaceNode);
}

Collector::Collector(const char *rootPath, int interfaceOther, int interfaceNode) :
        Component(rootPath) {

    host = new CollectorHost(getRootPath());

    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_NODE, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg));

    addProcessHandler(COMP_NODE, MSGTYPE_INFO, static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileInfoMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_BINARY, static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileBinaryMsg));

    initInterfaces(COMP_COLLECTOR, interfaceOther, interfaceNode);
}

Collector::~Collector() = default;

bool Collector::processDistributorWakeupMsg(ComponentUnit& owner, Message *msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(ComponentUnit& owner, Message *msg) {

    if (!setID(msg->getHeader().getVariant(0))) {

        return false;
    }

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Collector::processDistributorNodeMsg(ComponentUnit& owner, Message *msg) {

    TypeComponentList& nodes = msg->getData().getComponentList();

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Distributor assigned %d node", nodes.size());

    if (nodes.empty()) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "No available node is came from Distributor", nodes.size());
        delete msg;
        return false;
    }

    for (auto &node : nodes) {
        ProcessItem *process = job->assignNode(node);
        send2NodeProcessMsg(node, process);
    }

    return true;
}

bool Collector::processNodeFileInfoMsg(ComponentUnit& owner, Message *msg) {

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d]:Process[%d] requested %d missing files",
            owner.getID(), msg->getData().getFileProcess(), msg->getData().getFileCount());

    auto *processItem = job->getProcessByID(msg->getData().getFileProcess());
    if (!processItem) {

        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "Process[%d] could not find in the Job!", msg->getData().getFileProcess());
        return false;
    }

    TypeProcessFileList newList;

    for (auto file : msg->getData().getFileList()) {

        newList.emplace_back(processItem->getFile(file));
    }

    return send2NodeBinaryMsg(owner, processItem->getID(), newList);
}

bool Collector::processNodeFileBinaryMsg(ComponentUnit& owner, Message *msg) {

    auto& collectorHost = (CollectorHost&) getHost();

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] sent %d File output binaries", owner.getID(), msg->getData().getFileCount());

    int totalCount = 0;
    int readyCount = job->updateDependency(msg->getData().getFileProcess(), totalCount);

    send2NodeReadyMsg(owner);

    if (readyCount) {

        return send2DistributorNodeMsg(distributor, readyCount);
    }

    if (totalCount == 0) {

        collectorHost.setState(COLLSTATE_IDLE);

        job->setDuration(componentWatch.stop());

        notifyUI(NOTIFYSTATE_PASSIVE);

        return send2DistributorReadyMsg(distributor);
    }

    return true;
}

bool Collector::send2DistributorAliveMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Collector::send2DistributorIDMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_ID);

    return send(target, msg);
}

bool Collector::send2DistributorNodeMsg(ComponentUnit& target, long readyProcessCount) {

    auto *msg = new Message(getHost(), target, MSGTYPE_NODE);

    msg->getHeader().setVariant(0, readyProcessCount);

    return send(target, msg);
}

bool Collector::send2DistributorReadyMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_READY);

    return send(target, msg);
}

bool Collector::send2NodeProcessMsg(ComponentUnit& target, ProcessItem *processItem) {

    auto *msg = new Message(getHost(), target, MSGTYPE_PROCESS);

    msg->getData().setStreamFlag(STREAM_PROCESS);
    msg->getData().addProcess(processItem);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Node[%d]:Process[%d] is triggered ", target.getID(), processItem->getID());

    return send(target, msg);
}

bool Collector::send2NodeBinaryMsg(ComponentUnit& target, long processID, TypeProcessFileList &fileList) {

    auto *msg = new Message(getHost(), target, MSGTYPE_BINARY);

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(processID, fileList);

    return send(target, msg);
}

bool Collector::send2NodeReadyMsg(ComponentUnit& target) {

    auto *msg = new Message(getHost(), target, MSGTYPE_READY);

    return send(target, msg);
}

void Collector::setDistributor(const ComponentUnit& _distributor) {

    distributor = _distributor;
}

bool Collector::processJob() {

    auto& collectorHost = (CollectorHost&) getHost();

    if (job->getStatus() != JOBSTATUS_OK) {

        LOGS_E(getHost(), "Job : %s is not a valid job!!!", job->getJobName());
        return false;
    }

    componentWatch.start();

    notifyUI(NOTIFYSTATE_ACTIVE);

    collectorHost.setState(COLLSTATE_BUSY);

    return send2DistributorNodeMsg(distributor, job->getProcessCount(PROCESS_STATE_READY));
}

JobItem* Collector::loadJob(const char* zipFile) {

    delete job;

    try {

        job = new JobItem(getHost(), zipFile, JobItem::jobID++);

    } catch (std::exception &e) {

        LOGS_E(getHost(), "%s", e.what());

    }

    notifyUI(NOTIFYSTATE_ONCE);

    return job;
}

JobItem *Collector::getJob() {

    return job;
}
