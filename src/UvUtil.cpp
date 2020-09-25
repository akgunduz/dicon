//
// Created by akgun on 18.09.2020.
//

#include "UvUtil.h"
#include "Log.h"

void UvUtil::onAlloc(uv_handle_t* handle, size_t size, uv_buf_t* buf) {

    buf->base = (char *) malloc(size);

    assert(buf->base != nullptr);

    buf->len = size;

    //LOGP_E("%s => Allocated Buffer, Pointer : %p, Len : %d !!!", id, buf->base, size);

}

void UvUtil::onFree(const uv_buf_t *buf) {

    //LOGP_E("DeAllocating Buffer, Pointer : %p", buf->base);

    free(buf->base);

}

void UvUtil::onClose(uv_handle_t* handle) {

    if (!handle) {
        return;
    }

    if (uv_is_closing(handle)) {
        return;
    }

    uv_close(handle, [] (uv_handle_t* _handle) {

        if (_handle->data != nullptr) {

            delete (UserData*)(_handle->data);
        }

        free(_handle);
    });

}

void UvUtil::onShutdown(uv_stream_t* client) {

    auto *shutdown_req = (uv_shutdown_t *) calloc(1, sizeof(uv_shutdown_t));

    uv_shutdown(shutdown_req, client, [](uv_shutdown_t *req, int status) {

        if (status) {

            LOGP_E("Shutdown problem : %d!!!", status);
        }

        onClose((uv_handle_t*)req->handle);

        free(req);
    });
}

void UvUtil::onCloseAll(uv_loop_t *loop) {

    uv_walk(loop, [](uv_handle_t *handle, void *arg) {

        if (!uv_is_closing(handle)) {
            onClose(handle);
        }

    }, nullptr);
}

bool UvUtil::executeProcess(const std::string& parsedCmd, uv_loop_t* loop, void *data, TypeOnProcessSuccess onProcessSuccessCB) {

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

    auto processOptions = (uv_process_options_t *) calloc(1, sizeof(uv_process_options_t));
    auto child_stdio = (uv_stdio_container_t *) calloc(1, sizeof(uv_stdio_container_t) * 3);

    processOptions->stdio_count = 3;
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = UV_IGNORE;
//    child_stdio[1].flags = UV_INHERIT_FD;
//    child_stdio[1].data.fd = 1;
    child_stdio[2].flags = UV_IGNORE;
//    child_stdio[2].flags = UV_INHERIT_FD;
//    child_stdio[2].data.fd = 2;
    processOptions->stdio = child_stdio;
    processOptions->file = cmdArg[0];
    processOptions->args = cmdArg;
    processOptions->exit_cb = onProcessExit;

    return onExecuteProcess(loop, new ProcessData(parsedCmd, processOptions, data, onProcessSuccessCB));
}

bool UvUtil::onExecuteProcess(uv_loop_t* loop, ProcessData* processData) {

    auto childProcess = (uv_process_t *) calloc(1, sizeof(uv_process_t));

    childProcess->data = processData;

    int tryCount = 1;
    while(tryCount++ < PROCESS_TRY_COUNT) {

        int result = uv_spawn(loop, childProcess, processData->processOptions);
        if (result != 0) {

            LOGP_E("Process Spawn Failed : err => %s, retrying", uv_err_name(result));

            std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME * tryCount));

            continue;
        }

        break;
    }

    if (tryCount == PROCESS_TRY_COUNT) {

        return false;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}

void UvUtil::onProcessExit(uv_process_t* childProcess, int64_t exit_status, int term_signal) {

    auto loop = childProcess->loop;

    auto processData = (ProcessData*) childProcess->data;

    if (exit_status || term_signal) {

        LOGP_E("Process Execution is failed, retrying");

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME));

        onExecuteProcess(loop, new ProcessData(processData));

        onClose((uv_handle_t*)childProcess);

        return;

    }

    processData->onProcessSuccessCB(processData->data);

    onClose((uv_handle_t*)childProcess);
}