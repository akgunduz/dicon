//
// Created by akgun on 18.09.2020.
//

#ifndef DICON_UVUTIL_H
#define DICON_UVUTIL_H


#include <uv.h>
#include "Common.h"

class UvUtil {

    public:

    static void onAlloc(uv_handle_t*, size_t,uv_buf_t*);
    static void onFree(const uv_buf_t *);
    static void onClose(uv_handle_t*);
    static void onShutdown(uv_stream_t*);
    static void onCloseAll(uv_loop_t*);
};


#endif //DICON_UVUTIL_H
