//
// Created by z003sc8r on 9/9/2020.
//

#ifndef DICON_PROCESSDATA_H
#define DICON_PROCESSDATA_H

#include <string>
#include <cstring>
#include <system_error>
#include <filesystem>
#include <utility>
#include <uv.h>
#include "UserData.h"
#include "Platform.h"

typedef bool (*TypeOnProcessSuccess)(void*);

class ProcessData : public UserData {

    char cmdPath[PATH_MAX]{};
    char *cmdArg[MAX_INPUT]{};
    char executable[MAX_INPUT]{};

    uv_stdio_container_t child_stdio[3]{};

    uv_exit_cb onExitCB;

public:

    const std::string processCmd;

    uv_process_options_t processOptions{};

    const TypeOnProcessSuccess onProcessSuccessCB;

    ProcessData(std::string _processCmd, void *data, uv_exit_cb _onExitCB, const TypeOnProcessSuccess& _onProcessSuccessCB)
        : UserData(data), processCmd(std::move(_processCmd)), onExitCB(_onExitCB), onProcessSuccessCB(_onProcessSuccessCB) {

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
        processOptions.file = executable;
        processOptions.args = cmdArg;
        processOptions.exit_cb = onExitCB;
    }

    void parseCommand() {

        int index = 0;
        strcpy(cmdPath, processCmd.c_str());

        char *token = strtok(cmdPath, " ");
        strcpy(executable, token);
        strcat(executable, EXEC_EXT);
        std::error_code ec;
        std::filesystem::rename(token, executable, ec);
        std::filesystem::permissions(executable,
                                     std::filesystem::perms::owner_all |
                                     std::filesystem::perms::group_read |
                                     std::filesystem::perms::group_exec |
                                     std::filesystem::perms::others_read |
                                     std::filesystem::perms::others_exec,
                                     std::filesystem::perm_options::add);

        while(token) {

            cmdArg[index++] = token;
            token = strtok(nullptr, " ");
        }

        cmdArg[index] = nullptr;
    }
};


#endif //DICON_PROCESSDATA_H
