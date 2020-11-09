//
// Created by Haluk AKGUNDUZ on 18.09.2020.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#include <cassert>
#include <thread>
#include "UtilUV.h"
#include "Log.h"

void UtilUV::onAlloc(uv_handle_t* handle, size_t size, uv_buf_t* buf) {

    buf->base = (char *) malloc(size);

    assert(buf->base != nullptr);

    buf->len = size;

    //LOGP_E("%s => Allocated Buffer, Pointer : %p, Len : %d !!!", id, buf->base, size);

}

void UtilUV::onFree(const uv_buf_t *buf) {

    //LOGP_E("DeAllocating Buffer, Pointer : %p", buf->base);

    free(buf->base);

}

void UtilUV::onClose(uv_handle_t* handle) {

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

void UtilUV::onShutdown(uv_stream_t* client) {

    auto *shutdown_req = (uv_shutdown_t *) calloc(1, sizeof(uv_shutdown_t));

    int status = uv_shutdown(shutdown_req, client, [](uv_shutdown_t *req, int status) {

        if (status) {

            LOGP_E("Shutdown req problem : %s!!!", uv_err_name(status));
        }

        onClose((uv_handle_t*)req->handle);

        free(req);
    });

    if (status) {

        //LOGP_E("Shutdown problem : %s!!!", uv_err_name(status));

        free(shutdown_req);
    }
}

void UtilUV::onCloseAll(uv_loop_t *loop) {

    uv_walk(loop, [](uv_handle_t *handle, void *arg) {

        if (!uv_is_closing(handle)) {
            onClose(handle);
        }

    }, nullptr);
}

bool UtilUV::executeProcess(const std::string& parsedCmd, uv_loop_t* loop, void *data, TypeOnProcessSuccess onProcessSuccessCB) {

    return onExecuteProcess(loop, new ProcessData(parsedCmd, data, onProcessExit, onProcessSuccessCB));
}

bool UtilUV::onExecuteProcess(uv_loop_t* loop, ProcessData* processData) {

    auto childProcess = (uv_process_t *) calloc(1, sizeof(uv_process_t));
    if (childProcess) {
        childProcess->data = processData;
    }

    int tryCount = 1;
    while(tryCount++ < PROCESS_TRY_COUNT) {

        int result = uv_spawn(loop, childProcess, &processData->processOptions);
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

void UtilUV::onProcessExit(uv_process_t* childProcess, int64_t exit_status, int term_signal) {

    auto loop = childProcess->loop;

    auto processData = (ProcessData*) childProcess->data;

    if (exit_status || term_signal) {

        LOGP_E("Process Execution is failed, retrying");

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME));

        onExecuteProcess(loop, new ProcessData(processData));

    } else {

        processData->onProcessSuccessCB(processData->data);

    }

    onClose((uv_handle_t*)childProcess);
}