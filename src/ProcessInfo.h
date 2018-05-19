//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#ifndef BANKOR_PROCESSINFO_H
#define BANKOR_PROCESSINFO_H


#include "ProcessItem.h"

enum PROCESS_STATE {
    PROCESS_STATE_NOTSTARTED,
    PROCESS_STATE_STARTED,
    PROCESS_STATE_ENDED,
    PROCESS_STATE_MAX,
};

class ProcessInfo {

    ProcessItem *item;
    long id;
    PROCESS_STATE state;

public:

    ProcessInfo(long, ProcessItem *);
    ProcessItem* get();
    long getID();
    PROCESS_STATE getState();
    void setState(PROCESS_STATE);
};


#endif //BANKOR_PROCESSINFO_H
