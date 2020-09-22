//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "Util.h"

#define PROCESS_SLEEP_TIME 1000

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

    int status = uv_loop_init(&processLoop);
    if (status != 0) {

        LOGS_E(getHost(), "Process Loop can not created, returning, status : %s !!!",
               uv_err_name(status));

        return;
    }

    initialized = true;

    LOGS_T(getHost(), "Node is initialized");
}

Node::~Node() {

    LOGP_T("Deallocating Node");

    uv_loop_close(&processLoop);
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

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] binaries are received",
           owner->getID(), processItem->getID());

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Processing....",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

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

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGS_I(getHost(), "Collector[%d]:Process[%d] starts execution",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    std::string parsedCmd = Util::parsePath(getHost()->getRootPath(), processItem->getParsedProcess());

    std::filesystem::path executable = parsedCmd.substr(0, parsedCmd.find(' '));

    std::filesystem::permissions(executable,
                                 std::filesystem::perms::owner_all |
                                 std::filesystem::perms::group_read |
                                 std::filesystem::perms::group_exec |
                                 std::filesystem::perms::others_read |
                                 std::filesystem::perms::others_exec,
                                 std::filesystem::perm_options::add);

    LOGS_I(getHost(), "Collector[%d]:Process[%d] Command : %s",
           nodeHost->getAssigned()->getID(), processItem->getID(), parsedCmd.c_str());

    char cmdPath[PATH_MAX];

    int index = 0;
    strcpy(cmdPath, parsedCmd.c_str());

    char *token = strtok(cmdPath, " ");

    while(token) {

        processCmdArg[index++] = token;
        token = strtok(nullptr, " ");
    }

    processCmdArg[index] = nullptr;

    return executeProcess();
}

bool Node::executeProcess() {

    auto childProcess = (uv_process_t *) malloc(sizeof(uv_process_t));
    childProcess->data = this;

    uv_process_options_t processOptions{};
    processOptions.stdio_count = 3;
    uv_stdio_container_t child_stdio[3];
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = UV_IGNORE;
//    child_stdio[1].flags = UV_INHERIT_FD;
//    child_stdio[1].data.fd = 1;
    child_stdio[2].flags = UV_IGNORE;
//    child_stdio[2].flags = UV_INHERIT_FD;
//    child_stdio[2].data.fd = 2;
    processOptions.stdio = child_stdio;
    processOptions.file = processCmdArg[0];
    processOptions.args = processCmdArg;
    processOptions.exit_cb = onProcessExit;

    int tryCount = 1;
    while(tryCount++ < TRY_COUNT) {

        int result = uv_spawn(&processLoop, childProcess, &processOptions);
        if (result != 0) {

            LOGS_E(getHost(), "Process Spawn Failed : err => %s, retrying", uv_err_name(result));

            std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME * tryCount));

            continue;
        }

        break;
    }

    if (tryCount == TRY_COUNT) {

        return false;
    }

    return uv_run(&processLoop, UV_RUN_DEFAULT);
}

void Node::onProcessExit(uv_process_t* childProcess, int64_t exit_status, int term_signal) {

    auto node = (Node*) childProcess->data;

    childProcess->data = nullptr;
    UvUtil::onClose((uv_handle_t*)childProcess);

    if (exit_status || term_signal) {

        LOGS_E(node->getHost(), "Process Execution is failed, retrying");

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME));

        node->executeProcess();

        return;

    }

    node->onProcessSuccess();
}

bool Node::onProcessSuccess() {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    TypeProcessFileList outputList;

    for (const auto& processFile : processItem->getFileList()) {
        if (processFile->isOutput()) {
            if (processFile->get()->check()) {
                processFile->setOutputState(false);
                outputList.emplace_back(processFile);
            }
        }
    }

    return send2CollectorBinaryMsg(nodeHost->getAssigned(), processItem->getID(),outputList);
}

TypeProcessList& Node::getProcessList() {

    return processList;
}

