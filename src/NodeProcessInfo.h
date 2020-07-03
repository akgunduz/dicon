//
// Created by akgun on 3.07.2020.
//

#ifndef DICON_NODEPROCESSINFO_H
#define DICON_NODEPROCESSINFO_H

#include "Common.h"

class NodeProcessInfo {

    int attachedCollID;

    std::string jobDir;

    int processID;

public:

    int getAttachedColl();
    void setAttachedColl(int);

    int getProcessID();
    void setProcessID(int);

    std::string getJobID();
    void setJobID(std::string);

};

#endif //DICON_NODEPROCESSINFO_H
