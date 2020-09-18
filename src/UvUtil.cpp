//
// Created by akgun on 18.09.2020.
//

#include "UvUtil.h"
#include "Log.h"
#include "CommData.h"

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

            //TODO change CommData
            delete (CommData*)(_handle->data);
        }

        free(_handle);
    });

}

void UvUtil::onShutdown(uv_stream_t* client) {

    auto *shutdown_req = (uv_shutdown_t *) malloc(sizeof(uv_shutdown_t));

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