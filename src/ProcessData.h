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

public:

    const std::string processCmd;

    uv_process_options_t *processOptions;

    const TypeOnProcessSuccess onProcessSuccessCB;

    ProcessData(const std::string& _processCmd, uv_process_options_t *_processOptions,
                void *data, const TypeOnProcessSuccess& _onProcessSuccessCB)
        : UserData(data), processCmd(_processCmd),
            processOptions(_processOptions), onProcessSuccessCB(_onProcessSuccessCB) {
    }

    ProcessData(ProcessData *copy)
            : UserData(copy->data), processCmd(copy->processCmd),
              processOptions(copy->processOptions), onProcessSuccessCB(copy->onProcessSuccessCB) {
    }

    ~ProcessData() override {
        free(processOptions->stdio);
        free(processOptions);
    }
};


#endif //DICON_COMMDATA_H
