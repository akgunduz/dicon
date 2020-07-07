//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#ifndef DICON_PROCESSINFO_H
#define DICON_PROCESSINFO_H


#include "ProcessItem.h"

enum PROCESS_STATE {
    PROCESS_STATE_DEPENDENT,
    PROCESS_STATE_READY,
    PROCESS_STATE_REQUESTED,
    PROCESS_STATE_STARTED,
    PROCESS_STATE_ENDED,
    PROCESS_STATE_MAX,
};

class ProcessInfo {

    int id;
    PROCESS_STATE state;
    ProcessItem item;
    std::string jobDir;

    int assigned;

public:

    static ProcessInfo invalid;

    ProcessInfo(int, ProcessItem&);
    ProcessInfo(int);
    ProcessInfo(const ProcessInfo&);

    ProcessItem& get() const;

    int getID() const;
    void setID(int);

    PROCESS_STATE getState() const;
    void setState(PROCESS_STATE);

    std::string getJobID();
    void setJobID(std::string);

    int getAssigned() const;
    void setAssigned(int);
};


#endif //DICON_PROCESSINFO_H
