//
// Created by z003sc8r on 9/9/2020.
//

#ifndef DICON_PROCESSDATA_H
#define DICON_PROCESSDATA_H

#include "Message.h"
#include "CommInterface.h"
#include "UserData.h"

typedef bool (*TypeOnProcessSuccess)(void*);

class ProcessData : public UserData {

    char cmdPath[PATH_MAX]{};
    char *cmdArg[MAX_INPUT]{};

    uv_stdio_container_t child_stdio[3]{};

    uv_exit_cb onExitCB;

public:

    const std::string processCmd;

    uv_process_options_t processOptions{};

    const TypeOnProcessSuccess onProcessSuccessCB;

    ProcessData(const std::string& _processCmd, void *data, uv_exit_cb _onExitCB, const TypeOnProcessSuccess& _onProcessSuccessCB)
        : UserData(data), processCmd(_processCmd), onExitCB(_onExitCB), onProcessSuccessCB(_onProcessSuccessCB) {

        parseCommand();

        initOptions();

    }

    explicit ProcessData(ProcessData *copy)
            : UserData(copy->data), processCmd(copy->processCmd), onExitCB(copy->onExitCB),
              onProcessSuccessCB(copy->onProcessSuccessCB) {

        parseCommand();

        initOptions();
    }

    void initOptions() {

        processOptions.stdio_count = 3;
        child_stdio[0].flags = UV_IGNORE;
        child_stdio[1].flags = UV_IGNORE;
//    child_stdio[1].flags = UV_INHERIT_FD;
//    child_stdio[1].data.fd = 1;
        child_stdio[2].flags = UV_IGNORE;
//    child_stdio[2].flags = UV_INHERIT_FD;
//    child_stdio[2].data.fd = 2;
        processOptions.stdio = child_stdio;
        processOptions.file = cmdArg[0];
        processOptions.args = cmdArg;
        processOptions.exit_cb = onExitCB;
    }

    void parseCommand() {

        int index = 0;
        strcpy(cmdPath, processCmd.c_str());

        char *token = strtok(cmdPath, " ");

        while(token) {

            cmdArg[index++] = token;
            token = strtok(nullptr, " ");
        }

        cmdArg[index] = nullptr;
    }
};


#endif //DICON_COMMDATA_H
