//
// Created by akgun on 9.07.2020.
//

#ifndef DICON_PROCESSSTATE_H
#define DICON_PROCESSSTATE_H

enum PROCESS_STATE {
    PROCESS_STATE_DEPENDENT,
    PROCESS_STATE_READY,
    PROCESS_STATE_STARTED,
    PROCESS_STATE_ENDED,
    PROCESS_STATE_MAX,
};

class ProcessState {

public:
    static const char* getName(PROCESS_STATE);
};


#endif //DICON_PROCESSSTATE_H
