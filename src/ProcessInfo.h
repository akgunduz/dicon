//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#ifndef DICON_PROCESSINFO_H
#define DICON_PROCESSINFO_H

#include "ProcessState.h"
#include "ProcessItem.h"

class ProcessInfo {

    int id;
    PROCESS_STATE state;
    ProcessItem item;
    std::string jobName;

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

    std::string getJobName() const;
    void setJobName(std::string _jobName);

    int getAssigned() const;
    void setAssigned(int);
};


#endif //DICON_PROCESSINFO_H
