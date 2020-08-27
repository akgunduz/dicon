//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "CollectorHost.h"

Collector::Collector(int _commInterfaceOther, int _commInterfaceNode) {

    host = std::make_shared<CollectorHost>();

    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_NODE, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_REPLACE, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorReplaceMsg));

    addProcessHandler(COMP_NODE, MSGTYPE_INFO, static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileInfoMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_BINARY, static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileBinaryMsg));

    if (!initInterfaces(COMP_COLLECTOR, _commInterfaceOther, _commInterfaceNode)) {
        LOGS_E(getHost(), "Collector could not initialized");
        return;
    }

    initialized = true;

    LOGS_T(getHost(), "Collector is initialized");
}

Collector::~Collector() {

    LOGP_T("Deallocating Collector");
}

bool Collector::processDistributorWakeupMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    if (!setID((long)msg->getHeader().getVariant(0))) {

        return false;
    }

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "New ID : %d is assigned by Distributor", getHost()->getID());

    return send2DistributorIDMsg(owner);
}

bool Collector::processDistributorNodeMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    TypeComponentUnitList& nodes = msg->getData().getComponentList();

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Distributor assigned %d node", nodes.size());

    if (nodes.empty()) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "No available node is came from Distributor", nodes.size());
        return false;
    }

    for (const auto& node : nodes) {

        auto process = job->assignNode(node->getID());

        send2NodeProcessMsg(node, process);
    }

    return true;
}

bool Collector::processDistributorReplaceMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    TypeComponentUnitList& nodes = msg->getData().getComponentList();

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Distributor reassigned %d node", nodes.size() / 2);

    if (nodes.size() < 2) {
        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "At least two node info should came, old and new node IDs");
        return false;
    }

    for (size_t i = 0; i < nodes.size(); i = i + 2) {

        auto process = job->reAssignNode(nodes[i]->getID(), nodes[i + 1]->getID());

        if (process) {

            send2NodeProcessMsg(nodes[i + 1], process);
        }
    }

    return true;
}

bool Collector::processNodeFileInfoMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d]:Process[%d] requested %d missing files",
            owner->getID(), msg->getData().getProcess()->getID(), msg->getData().getProcess()->getFileCount());

    auto processItem = job->getProcessByID(msg->getData().getProcess()->getID());

    if (!processItem) {

        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "Process[%d] could not find in the Job!",
               msg->getData().getProcess()->getID());

        return false;
    }

    TypeProcessFileList newList;

    for (const auto& file : msg->getData().getProcess()->getFileList()) {

        newList.emplace_back(processItem->getFile(file));
    }

    return send2NodeBinaryMsg(owner, processItem->getID(), newList);
}

bool Collector::processNodeFileBinaryMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    auto collectorHost = std::static_pointer_cast<CollectorHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] sent %d File output binaries",
           owner->getID(), msg->getData().getProcess()->getFileCount());

    int totalCount = 0;
    int readyCount = job->updateDependency(msg->getData().getProcess()->getID(), totalCount);

    send2NodeReadyMsg(owner);

    if (readyCount) {

        return send2DistributorNodeMsg(distributor, readyCount);
    }

    if (totalCount == 0) {

        collectorHost->setState(COLLSTATE_IDLE);

        job->setDuration(componentWatch.stop());

        notifyUI(NOTIFYTYPE_PASSIVE);

        return send2DistributorReadyMsg(distributor);
    }

    return true;
}

bool Collector::send2DistributorAliveMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_ALIVE);

    return send(target, std::move(msg));
}

bool Collector::send2DistributorIDMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_ID);

    return send(target, std::move(msg));
}

bool Collector::send2DistributorNodeMsg(const TypeComponentUnit& target, long readyProcessCount) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_NODE);

    msg->getHeader().setVariant(0, readyProcessCount);

    return send(target, std::move(msg));
}

bool Collector::send2DistributorReadyMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_READY);

    return send(target, std::move(msg));
}

bool Collector::send2NodeProcessMsg(const TypeComponentUnit& target, const TypeProcessItem& processItem) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_PROCESS);

    msg->getData().setStreamType(STREAM_PROCESS);
    msg->getData().setProcess(processItem);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Node[%d]:Process[%d] is triggered ",
           target->getID(), processItem->getID());

    return send(target, std::move(msg));
}

bool Collector::send2NodeBinaryMsg(const TypeComponentUnit& target, long processID, const TypeProcessFileList &fileList) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_BINARY);

    msg->getData().setStreamType(STREAM_FILEBINARY);
    msg->getData().setProcess(processID, fileList);

    return send(target, std::move(msg));
}

bool Collector::send2NodeReadyMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_READY);

    return send(target, std::move(msg));
}

void Collector::setDistributor(const TypeComponentUnit& _distributor) {

    distributor = _distributor;
}

bool Collector::processJob() {

    auto collectorHost = std::static_pointer_cast<CollectorHost>(host);

    if (job->getStatus() != JOBSTATUS_OK) {

        return false;
    }

    componentWatch.start();

    notifyUI(NOTIFYTYPE_ACTIVE);

    collectorHost->setState(COLLSTATE_BUSY);

    return send2DistributorNodeMsg(distributor, job->getProcessCount(PROCESS_STATE_READY));
}

TypeJobItem& Collector::loadJob(const std::filesystem::path& zipFile) {

    job = std::make_shared<JobItem>(getHost(), zipFile, JobItem::jobID++);

    notifyUI(NOTIFYTYPE_ONCE);

    return job;
}

TypeJobItem& Collector::getJob() {

    return job;
}
