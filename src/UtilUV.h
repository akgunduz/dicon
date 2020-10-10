//
// Created by akgun on 18.09.2020.
//

#ifndef DICON_UTILUV_H
#define DICON_UTILUV_H

#include <uv.h>
#include "ProcessData.h"

#define PROCESS_SLEEP_TIME 1000
#define PROCESS_TRY_COUNT 10

class UtilUV {

    public:

    static void onAlloc(uv_handle_t*, size_t,uv_buf_t*);
    static void onFree(const uv_buf_t *);
    static void onClose(uv_handle_t*);
    static void onShutdown(uv_stream_t*);
    static void onCloseAll(uv_loop_t*);
    static bool executeProcess(const std::string& parsedCmd, uv_loop_t* loop, void *data, TypeOnProcessSuccess onProcessSuccessCB);
    static bool onExecuteProcess(uv_loop_t* loop, ProcessData*);
    static void onProcessExit(uv_process_t* childProcess, int64_t exit_status, int term_signal);
};


#endif //DICON_UTILUV_H
