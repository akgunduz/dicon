//
// Created by Haluk AKGUNDUZ on 22.07.2020.
//

#ifndef DICON_PROCESSFILE_H
#define DICON_PROCESSFILE_H


#include "FileItem.h"

class ProcessFile {

    bool is_output{};
    long processID{};
    FileItem *content{};

public:

    ProcessFile(FileItem*, long = 0, bool = false);
    bool isOutput() const;
    FileItem* get();
    long getAssignedProcess();
    void setAssignedProcess(long _processID);
    void setOutputState(bool);
};


#endif //DICON_PROCESSFILE_H
