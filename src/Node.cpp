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

  //  processLoop = uv_default_loop();

    initialized = true;

    LOGS_T(getHost(), "Node is initialized");
}

Node::~Node() {

    LOGP_T("Deallocating Node");

   // uv_loop_close(processLoop);
};

bool Node::processDistributorWakeupMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Setting Distributor");

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(const TypeComponentUnit& owner, TypeMessage msg) {

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

        return processJob(owner, std::move(msg));
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

    return processJob(owner, std::move(msg));
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

bool Node::processJob(const TypeComponentUnit& owner, TypeMessage msg) {

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

#if 0
    char cmdPath[PATH_MAX];
    char *cmdArg[MAX_INPUT];
    int index = 0;
    strcpy(cmdPath, parsedCmd.c_str());

    char *token = strtok(cmdPath, " ");

    while(token) {

        cmdArg[index++] = token;
        token = strtok(nullptr, " ");
    }

    cmdArg[index] = nullptr;

    auto *childProcess = (uv_process_t *) malloc(sizeof(uv_process_t));
    childProcess->data = this;

    uv_process_options_t options{};
    options.stdio_count = 3;
    uv_stdio_container_t child_stdio[3];
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = UV_IGNORE;
//    child_stdio[1].data.fd = 1;
    child_stdio[2].flags = UV_IGNORE;
//    child_stdio[2].data.fd = 2;
    options.stdio = child_stdio;

    options.exit_cb = [] (uv_process_t* handle, int64_t exit_status, int term_signal) {

        auto node = (Node*) handle->data;

        LOGP_E("PROCESS[%d], EXIT STATUS : %ld, TERM : %d", node->processItem->getID(), exit_status, term_signal);

        ((Node*) handle->data)->onProcessSuccess();
        handle->data = nullptr;

        UvUtil::onClose((uv_handle_t*)handle);
      // UvUtil::onCloseAll(handle->loop);
    };

    options.file = cmdArg[0];
    options.args = cmdArg;

    if (processItem->getID() == 12 || processItem->getID() == 7) {
        LOGP_E("PROCESS %d starting.................", processItem->getID());
    }
    int result = uv_spawn(processLoop, childProcess, &options);
    if (result != 0) {
        LOGP_E("Process Spawn Failed : err", uv_err_name(result));
        return false;
    }

    result = uv_run(processLoop, UV_RUN_ONCE);
    LOGP_E("Process Loop Status : %d", result);

    return true;

#else
    std::string childOut;

    int tryCount = 1;
    while(tryCount++ < TRY_COUNT) {

        redi::ipstream inProcess(parsedCmd);

        if (!inProcess.good()) {
            LOGS_E(getHost(), "Collector[%d]:Process[%d] Can not execute command",
                   nodeHost->getAssigned()->getID(), processItem->getID(), parsedCmd.c_str());
            return false;
        }

        while (std::getline(inProcess, childOut)) {
            Util::trim(childOut);
            if (!childOut.empty()) {
                LOGS_I(getHost(), "Collector[%d]:Process[%d] Output : %s",
                       nodeHost->getAssigned()->getID(), processItem->getID(), childOut.c_str());
            }
        }

        int res = inProcess.close();
        if (res == 0) {

            return onProcessSuccess();
        }

        LOGS_E(getHost(), "Collector[%d]:Process[%d] has execution problem retrying",
               nodeHost->getAssigned()->getID(), processItem->getID());

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME * tryCount));
    }

    LOGS_E(getHost(), "Collector[%d]:Process[%d] is ended with error",
           nodeHost->getAssigned()->getID(), processItem->getID());

    return false;
#endif

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

