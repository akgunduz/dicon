//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "CollectorObject.h"

#define PROCESS_SLEEP_TIME 1000

Node *Node::newInstance(const char* path) {

    return new Node(path);
}

Node::Node(const char *rootPath) :
        Component(rootPath), distributor(getRootPath()){

    host = new NodeObject(getRootPath());

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_PROCESS] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_JOB] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg);

    initInterfaces(COMP_NODE);
}

Node::~Node() = default;

bool Node::processDistributorWakeupMsg(const ComponentObject& owner, Message *msg) {

    setDistributor((DistributorObject &) owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(const ComponentObject& owner, Message *msg) {

    setID((int)msg->getHeader()->getVariant(0));

    notifyUI();
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processCollectorJobMsg(const ComponentObject& owner, Message *msg) {

    ((NodeObject&) getHost()).setState(NODESTATE_BUSY);
    ((NodeObject&) getHost()).setAssigned(owner.getID(), owner.getAddress());

    ((NodeObject&) getHost()).getProcessInfo().setAssigned(owner.getID());
    ((NodeObject&) getHost()).getProcessInfo().setState(PROCESS_STATE_STARTED);
    ((NodeObject&) getHost()).getProcessInfo().setJobName(msg->getData()->getJobName());
    ((NodeObject&) getHost()).getProcessInfo().setID(msg->getData()->getProcessID());
    ((NodeObject&) getHost()).getProcessInfo().get().setParsedProcess(msg->getData()->getProcess());
    ((NodeObject&) getHost()).getProcessInfo().get().setFileList(msg->getData()->getFileList());

    LOGS_I(getHost(), "Collector[%d]:Process[%d] request is received ",
           owner.getID(), msg->getData()->getProcessID());

    return send2DistributorBusyMsg(getDistributor(), owner.getID());
}

bool Node::processJob(const ComponentObject& owner, Message *msg) {

    LOGS_I(getHost(), "Collector[%d]:Process[%d] starts execution",
           ((NodeObject&)getHost()).getAssigned().getID(),
           ((NodeObject&)getHost()).getProcessInfo().getID());

    ProcessInfo &processInfo = ((NodeObject&)getHost()).getProcessInfo();

    processList.emplace_back(processInfo);

    notifyUI();

    int result = processCommand(((NodeObject&)getHost()).getAssigned().getID(),
            ((NodeObject&)getHost()).getProcessInfo().getID(),
            ((NodeObject &) getHost()).getProcessInfo().get().getParsedProcess());

    if (!result) {
        return false;
    }

    TypeFileInfoList outputList = FileInfo::getFileList(((NodeObject&)getHost()).getProcessInfo().get().getFileList(), true);
    FileInfo::setFileListState(outputList, false);

    LOGS_I(getHost(), "Collector[%d]:Process[%d] is successfully executed, sending back output data",
           ((NodeObject&)getHost()).getAssigned().getID(),
           ((NodeObject&)getHost()).getProcessInfo().getID());

    //TODO will update with md5s including outputs
    //TODO will update with actual binary to collector including outputs
    return send2CollectorBinaryMsg(CollectorObject(((NodeObject&)getHost()).getAssigned().getID(),
                                                   ((NodeObject&)getHost()).getAssigned().getAddress()),
                                   ((NodeObject &) getHost()).getProcessInfo().getJobName().c_str(),
                                   ((NodeObject&)getHost()).getProcessInfo().getID(),
                                   ((NodeObject &) getHost()).getProcessInfo().get().getParsedProcess(),
                                   outputList);
}

bool Node::processDistributorProcessMsg(const ComponentObject& owner, Message *msg) {

    LOGS_I(getHost(), "Collector[%d]:Process[%d] is approved by distributor",
           ((NodeObject&)getHost()).getAssigned().getID(),
           ((NodeObject&)getHost()).getProcessInfo().getID());

    TypeFileInfoList requiredList = FileInfo::getReqFileList(getHost(),
            ((NodeObject&)getHost()).getProcessInfo().get().getFileList());

    if (!requiredList.empty()) {

        LOGS_I(getHost(), "Collector[%d]:Process[%d]'s missing files are requested, total %d files",
               ((NodeObject&)getHost()).getAssigned().getID(),
               ((NodeObject&)getHost()).getProcessInfo().getID(),
               requiredList.size());

        return send2CollectorInfoMsg(CollectorObject(((NodeObject&)getHost()).getAssigned().getID(),
                                                     ((NodeObject&)getHost()).getAssigned().getAddress()),
                                     ((NodeObject &) getHost()).getProcessInfo().getJobName().c_str(),
                                     ((NodeObject&)getHost()).getProcessInfo().getID(),
                                     ((NodeObject &) getHost()).getProcessInfo().get().getParsedProcess(),
                                     requiredList);

    } else {

        return processJob(owner, msg);
    }
}


bool Node::processCollectorBinaryMsg(const ComponentObject& owner, Message *msg) {

    LOGS_I(getHost(), "Collector[%d]:Process[%d] binaries are received",
           owner.getID(), msg->getData()->getProcessID());

    return processJob(owner, msg);
}

bool Node::processCollectorReadyMsg(const ComponentObject& owner, Message *msg) {

    ((NodeObject&)getHost()).setState(NODESTATE_IDLE);

    notifyUI();

    return send2DistributorReadyMsg(getDistributor());
}

bool Node::send2DistributorReadyMsg(const ComponentObject& target) {

	auto *msg = new Message(getHost(), MSGTYPE_READY);

	return send(target, msg);
}

bool Node::send2DistributorAliveMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Node::send2DistributorIDMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    return send(target, msg);
}

bool Node::send2DistributorBusyMsg(const ComponentObject& target, int collID) {

    auto *msg = new Message(getHost(), MSGTYPE_BUSY);

    msg->getHeader()->setVariant(0, collID);

    return send(target, msg);
}

bool Node::send2CollectorInfoMsg(const ComponentObject& target, const char* jobName, int processID,
                                 const char* process, TypeFileInfoList &fileList) {

	auto *msg = new Message(getHost(), MSGTYPE_INFO);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobName(jobName);
    msg->getData()->setProcess(processID, process);
    msg->getData()->addFileList(fileList);

	return send(target, msg);
}

bool Node::send2CollectorBinaryMsg(const ComponentObject& target, const char* jobName, int processID,
                                   const char* process, TypeFileInfoList &fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobName(jobName);
    msg->getData()->setProcess(processID, process);
    msg->getData()->addFileList(fileList);

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

bool Node::processCommand(int collID, int processID, const char *cmd) {

    char fullCmd[PATH_MAX];

    strcpy(fullCmd, Util::parsePath(getHost().getRootPath(), cmd).c_str());

    LOGS_I(getHost(), "Collector[%d]:Process[%d] Command : %s", collID, processID, fullCmd);

    char childOut[256];
    int tryCount = 1;
    while(tryCount++ < 10) {

        FILE *fdProcess = popen(fullCmd, "r");
        if (fdProcess == nullptr) {
            LOGS_E(getHost(), "Collector[%d]:Process[%d] Can not execute command", collID, processID, fullCmd);
            return false;
        }

        while (fgets(childOut, 256, fdProcess)) {
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

std::vector<ProcessInfo> &Node::getProcessList() {

    return processList;
}
