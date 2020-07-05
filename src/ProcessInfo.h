//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#ifndef DICON_PROCESSINFO_H
#define DICON_PROCESSINFO_H


#include "ProcessItem.h"

enum PROCESS_STATE {
    PROCESS_STATE_DEPENDENT,
    PROCESS_STATE_READY,
    PROCESS_STATE_STARTED,
    PROCESS_STATE_ENDED,
    PROCESS_STATE_MAX,
};

class ProcessInfo {

    ProcessItem *item;
    int id;
    PROCESS_STATE state;
    int assigned;

public:

    static ProcessInfo invalid;

    ProcessInfo(int, ProcessItem *);
    ProcessItem* get() const;
    int getID() const;

    PROCESS_STATE getState() const;
    void setState(PROCESS_STATE);

    int getAssignedNode();
    void setAssignedNode(int);

};


#endif //DICON_PROCESSINFO_H
