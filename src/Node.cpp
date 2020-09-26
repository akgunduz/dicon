//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "Util.h"
#include "UserData.h"

Node::Node(int _commInterface) :
    Component(std::make_shared<NodeHost>()) {

    LOGS_T(getHost(), "Node is initializing");

    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP, static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_PROCESS, static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg));

    addProcessHandler(COMP_COLLECTOR, MSGTYPE_PROCESS, static_cast<TypeProcessComponentMsg>(&Node::processCollectorProcessMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_BINARY, static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg));

    if (!initInterfaces(COMP_NODE, _commInterface, _commInterface)) {
        LOGS_E(getHost(), "Node could not initialized");
        return;
    }

    processItem = std::make_shared<ProcessItem>(getHost());

    processThread = std::thread([](Node *node){

        node->spawnProcess();

    }, this);

    initialized = true;

    LOGS_T(getHost(), "Node is initialized");
}

Node::~Node() {

    LOGP_T("Deallocating Node");

    runProcessThread = false;

    processReady = true;

    cond.notify_one();

    processThread.join();
};

bool Node::processDistributorWakeupMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Setting Distributor");

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    setDistributor(owner);

    if (!setID(msg->getData().getID())) {

        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "New ID : 0 is tried to assigned, it should not!!!");

        return false;
    }

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "New ID : %d is assigned by Distributor", getHost()->getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processDistributorProcessMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] is approved by distributor",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    TypeProcessFileList requiredList;

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Figuring out required File List",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    for (const auto& processFile : processItem->getFileList()) {
        if (!processFile->isOutput() &&
                !std::filesystem::exists(nodeHost->getRootPath() /
                    std::to_string(processFile->get()->getAssignedJob()) /
                    processFile->get()->getName())) {
            requiredList.emplace_back(processFile);
        }
    }

    if (!requiredList.empty()) {

        LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Required %d Files is requested",
               nodeHost->getAssigned()->getID(),
               processItem->getID(), requiredList.size());

        return send2CollectorInfoMsg(nodeHost->getAssigned(), processItem->getID(),requiredList);

    } else {

        LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Processing....",
               nodeHost->getAssigned()->getID(),
               processItem->getID());

        return executeJob(owner, std::move(msg));
    }
}

bool Node::processCollectorProcessMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] request is received ",
           owner->getID(), processItem->getID());

    componentWatch.start();

    nodeHost->setState(NODESTATE_BUSY);
    nodeHost->setAssigned(owner->getType(), owner->getArch(), owner->getID(), owner->getAddress());

    processItem = msg->getData().getProcess();
    processItem->setAssigned(owner->getID());
    processItem->setState(PROCESS_STATE_STARTED);

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Prepared....",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    return send2DistributorBusyMsg(distributor, owner->getID());
}

bool Node::processCollectorBinaryMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] binaries are received, starting process",
           owner->getID(), processItem->getID());

    return executeJob(owner, std::move(msg));
}

bool Node::processCollectorReadyMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    nodeHost->setState(NODESTATE_IDLE);

    processItem->setState(PROCESS_STATE_ENDED);

    processItem->setDuration(componentWatch.stop());

    processList.emplace_back(processItem);

    notifyUI(NOTIFYTYPE_TRANSPARENT);

    return send2DistributorReadyMsg(distributor);
}

bool Node::send2DistributorReadyMsg(const TypeComponentUnit& target) {

	auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_READY);

	return send(target, std::move(msg));
}

bool Node::send2DistributorAliveMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_ALIVE);

    return send(target, std::move(msg));
}

bool Node::send2DistributorIDMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_ID);

    return send(target, std::move(msg));
}

bool Node::send2DistributorBusyMsg(const TypeComponentUnit& target, TypeID collID) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_BUSY, STREAM_ID);

    msg->getData().setID(collID);

    return send(target, std::move(msg));
}

bool Node::send2CollectorInfoMsg(const TypeComponentUnit& target, long processID, const TypeProcessFileList &fileList) {

	auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_INFO, STREAM_PROCESS_FILES_INFO);

    msg->getData().setProcess(processID, fileList);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Collector[%d]:Process[%d]'s missing files are requested, total %d files",
           target->getID(),
           processID,
           fileList.size());

	return send(target, std::move(msg));
}

bool Node::send2CollectorBinaryMsg(const TypeComponentUnit& target, long processID, const TypeProcessFileList &fileList) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_BINARY, STREAM_PROCESS_FILES_BINARY);

    msg->getData().setProcess(processID, fileList);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Collector[%d]:Process[%d] is successfully executed, sending back output data",
           target->getID(),
           processID);

    return send(target, std::move(msg));
}

void Node::setDistributor(const TypeComponentUnit& _distributor) {

    distributor = std::make_shared<ComponentUnit>(_distributor);
}

void Node::parseCommand(char *cmd, char **argv) {

    while (*cmd != '\0') {

        while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') {
            *cmd++ = '\0';
        }

        *argv++ = cmd;

        while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n') {
            cmd++;
        }
    }
    *argv = nullptr;
}

bool Node::executeJob(const TypeComponentUnit& owner, TypeMessage msg) {

    processReady = true;

    cond.notify_one();

    return true;
}

void Node::spawnProcess() {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    int status = uv_loop_init(&processLoop);
    if (status != 0) {

        LOGS_E(getHost(), "Process Loop can not created, returning, status : %s !!!",
               uv_err_name(status));

        return;
    }

    while (runProcessThread) {

        std::unique_lock<std::mutex> lock(mutex);

        cond.wait(lock, [this] {
            return processReady.load();
        });

        if (!runProcessThread) {
            break;
        }

        processReady = false;

        auto parsedCmd = Util::parsePath(getHost()->getRootPath(), processItem->getParsedProcess());

        LOGS_I(getHost(), "Collector[%d]:Process[%d] Command : %s",
               nodeHost->getAssigned()->getID(), processItem->getID(), parsedCmd.c_str());

        UvUtil::executeProcess(parsedCmd, &processLoop, this, onProcessSuccess);
    }

    uv_loop_close(&processLoop);
}

bool Node::onProcessSuccess(void *data) {

    auto node = (Node*) data;

    auto nodeHost = std::static_pointer_cast<NodeHost>(node->getHost());

    TypeProcessFileList outputList;

    for (const auto& processFile : node->processItem->getFileList()) {
        if (processFile->isOutput()) {
            if (processFile->get()->check()) {
                processFile->setOutputState(false);
                outputList.emplace_back(processFile);
            }
        }
    }

    return node->send2CollectorBinaryMsg(nodeHost->getAssigned(), node->processItem->getID(),outputList);
}

TypeProcessList& Node::getProcessList() {

    return processList;
}
