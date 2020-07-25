//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "CollectorObject.h"
#include "Util.h"

#define PROCESS_SLEEP_TIME 1000

Node *Node::newInstance(const char* path) {

    return new Node(path);
}

Node::Node(const char *rootPath) :
        Component(rootPath), distributor(getRootPath()){

    host = new NodeObject(getRootPath());

    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP, static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_PROCESS, static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg));

    addProcessHandler(COMP_COLLECTOR, MSGTYPE_PROCESS, static_cast<TypeProcessComponentMsg>(&Node::processCollectorProcessMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_BINARY, static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg));

    initInterfaces(COMP_NODE);
}

Node::~Node() = default;

bool Node::processDistributorWakeupMsg(const ComponentObject& owner, Message *msg) {

    setDistributor((DistributorObject &) owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(const ComponentObject& owner, Message *msg) {

    setID((int)msg->getHeader().getVariant(0));

    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processDistributorProcessMsg(const ComponentObject& owner, Message *msg) {

    auto& nodeHost = (NodeObject&) getHost();

    LOGS_I(getHost(), "Collector[%d]:Process[%d] is approved by distributor",
           nodeHost.getAssigned().getID(),
           nodeHost.getProcess().getID());

    TypeProcessFileList requiredList;

    for (auto processFile : nodeHost.getProcess().getFileList()) {
        if (!processFile.isOutput() &&
            !Util::checkPath(nodeHost.getRootPath(),
                             processFile.get()->getAssignedJob(),
                             processFile.get()->getName(), false)) {
            requiredList.emplace_back(processFile);
        }
    }

    if (!requiredList.empty()) {

        LOGS_I(getHost(), "Collector[%d]:Process[%d]'s missing files are requested, total %d files",
               nodeHost.getAssigned().getID(),
               nodeHost.getProcess().getID(),
               requiredList.size());

        return send2CollectorInfoMsg(CollectorObject(nodeHost.getAssigned().getID(),
                                                     nodeHost.getAssigned().getAddress()),
                                     nodeHost.getProcess().getID(),
                                     requiredList);

    } else {

        return processJob(owner, msg);
    }
}

bool Node::processCollectorProcessMsg(const ComponentObject& owner, Message *msg) {

    auto& nodeHost = (NodeObject&) getHost();

    nodeHost.setState(NODESTATE_BUSY);
    nodeHost.setAssigned(owner.getType(), owner.getID(), owner.getAddress());

    nodeHost.getProcess() = *msg->getData().getProcess(0);
    nodeHost.getProcess().setAssigned(owner.getID());
    nodeHost.getProcess().setState(PROCESS_STATE_STARTED);
    nodeHost.getProcess().addFileList(msg->getData().getFileList());

    LOGS_I(getHost(), "Collector[%d]:Process[%d] request is received ",
           owner.getID(), nodeHost.getProcess().getID());

    return send2DistributorBusyMsg(getDistributor(), owner.getID());
}

bool Node::processCollectorBinaryMsg(const ComponentObject& owner, Message *msg) {

    auto& nodeHost = (NodeObject&) getHost();

    LOGS_I(getHost(), "Collector[%d]:Process[%d] binaries are received",
           owner.getID(), nodeHost.getProcess().getID());

    return processJob(owner, msg);
}

bool Node::processCollectorReadyMsg(const ComponentObject& owner, Message *msg) {

    ((NodeObject&)getHost()).setState(NODESTATE_IDLE);

    notifyUI();

    return send2DistributorReadyMsg(getDistributor());
}


bool Node::processJob(const ComponentObject& owner, Message *msg) {

    auto& nodeHost = (NodeObject&) getHost();

    LOGS_I(getHost(), "Collector[%d]:Process[%d] starts execution",
           nodeHost.getAssigned().getID(),
           nodeHost.getProcess().getID());

    processList.emplace_back(nodeHost.getProcess());

    notifyUI();

    int result = processCommand(nodeHost.getAssigned().getID(),
                                nodeHost.getProcess().getID(),
                                nodeHost.getProcess().getParsedProcess());

    if (!result) {
        return false;
    }

    TypeProcessFileList outputList;

    for (auto processFile : nodeHost.getProcess().getFileList()) {
        if (processFile.isOutput()) {
            if (processFile.get()->check()) {
                processFile.setOutputState(false);
                outputList.emplace_back(processFile);
            }
        }
    }

    LOGS_I(getHost(), "Collector[%d]:Process[%d] is successfully executed, sending back output data",
           nodeHost.getAssigned().getID(),
           nodeHost.getProcess().getID());

    return send2CollectorBinaryMsg(CollectorObject(nodeHost.getAssigned().getID(),
                                                   nodeHost.getAssigned().getAddress()),
                                   nodeHost.getProcess().getID(),
                                   outputList);
}

bool Node::send2DistributorReadyMsg(const ComponentObject& target) {

	auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_READY);

	return send(target, msg);
}

bool Node::send2DistributorAliveMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Node::send2DistributorIDMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_ID);

    return send(target, msg);
}

bool Node::send2DistributorBusyMsg(const ComponentObject& target, long collID) {

    auto *msg = new Message(getHost(), COMP_DISTRIBUTOR, MSGTYPE_BUSY);

    msg->getHeader().setVariant(0, collID);

    return send(target, msg);
}

bool Node::send2CollectorInfoMsg(const ComponentObject& target, long processID, TypeProcessFileList &fileList) {

	auto *msg = new Message(getHost(), COMP_COLLECTOR, MSGTYPE_INFO);

    msg->getData().setStreamFlag(STREAM_FILEINFO);
    msg->getData().addFileList(processID, fileList);

	return send(target, msg);
}

bool Node::send2CollectorBinaryMsg(const ComponentObject& target, long processID, TypeProcessFileList &fileList) {

    auto *msg = new Message(getHost(), COMP_COLLECTOR, MSGTYPE_BINARY);

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(processID, fileList);

    return send(target, msg);
}

ComponentObject Node::getDistributor() {

    return distributor;
}

void Node::setDistributor(const DistributorObject& _distributor) {

    this->distributor = _distributor;
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

bool Node::processCommand(long collID, long processID, const char *cmd) {

    char fullCmd[PATH_MAX];

    strcpy(fullCmd, Util::parsePath(getHost().getRootPath(), cmd).c_str());

    LOGS_I(getHost(), "Collector[%d]:Process[%d] Command : %s", collID, processID, fullCmd);

    char childOut[PATH_MAX];
    int tryCount = 1;
    while(tryCount++ < 10) {

        FILE *fdProcess = popen(fullCmd, "r");
        if (fdProcess == nullptr) {
            LOGS_E(getHost(), "Collector[%d]:Process[%d] Can not execute command", collID, processID, fullCmd);
            return false;
        }

        while (fgets(childOut, PATH_MAX, fdProcess)) {
            const char *p = Util::trimEndLine(childOut);
            if (strcmp(p, "") != 0) {
                LOGS_I(getHost(), "Collector[%d]:Process[%d] Output : %s", collID, processID, p);
            }
        }

        int res = pclose(fdProcess);
        if (res == 0) {
            return true;
        }

        LOGS_E(getHost(), "Collector[%d]:Process[%d] has execution problem retrying", collID, processID);

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME * tryCount));
    }

    LOGS_E(getHost(), "Collector[%d]:Process[%d] is ended with error", collID, processID);

    return false;
}

std::vector<ProcessItem> &Node::getProcessList() {

    return processList;
}
