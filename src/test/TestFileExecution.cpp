//
// Created by akgun on 18.09.2020.
//

#include <pstreams/pstream.h>
#include "TestApp.h"

void TestApp::testFileExecute(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    auto job = std::make_shared<JobItem>(collector->getHost(), TEST_JOB_PATH, JobItem::jobID++);
    if (!job) {
        LOGP_E("Job could not initialized");
        return;
    }

    LOGP_I("Execute Ready Processes");

    auto process = job->getProcess(0);
    if (process->getState() != PROCESS_STATE_READY) {
        return;
    }

    std::string parsedCmd = Util::parsePath(collector->getHost()->getRootPath(), process->getParsedProcess());

    LOGP_I("\tID : %d, Process : %s, Parsed : %s",
           process->getID(),
           process->getProcess().c_str(),
           parsedCmd.c_str());

    std::filesystem::path executable = parsedCmd.substr(0, parsedCmd.find(' '));

    std::filesystem::permissions(executable,
                                 std::filesystem::perms::owner_all |
                                 std::filesystem::perms::group_read |
                                 std::filesystem::perms::group_exec |
                                 std::filesystem::perms::others_read |
                                 std::filesystem::perms::others_exec,
                                 std::filesystem::perm_options::add);


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
    uv_process_options_t options{};

    options.stdio_count = 3;
    uv_stdio_container_t child_stdio[3];
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = UV_INHERIT_FD;
    child_stdio[1].data.fd = 1;
    child_stdio[2].flags = UV_INHERIT_FD;
    child_stdio[2].data.fd = 2;
    options.stdio = child_stdio;

    options.exit_cb = [] (uv_process_t* handle, int64_t exit_status, int term_signal) {

        if (!handle) {
            return;
        }

        if (uv_is_closing((uv_handle_t*)handle)) {
            return;
        }

        uv_close((uv_handle_t*)handle, [] (uv_handle_t* _handle) {

            free(_handle);
        });
    };

    options.file = cmdArg[0];
    options.args = cmdArg;

    int result = uv_spawn(uv_default_loop(), childProcess, &options);
    if (result != 0) {
        LOGP_E("Process Spawn Failed : err", uv_err_name(result));
        return;
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_loop_close(uv_default_loop());
}
