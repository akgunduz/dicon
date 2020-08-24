//
// Created by Haluk AKGUNDUZ on 22.07.2020.
//

#ifndef DICON_PROCESSFILE_H
#define DICON_PROCESSFILE_H

#include "FileItem.h"

class ProcessFile {

    bool is_output{};

    long processID{};

    TypeFileItem content;

public:

    explicit ProcessFile(TypeFileItem, long = 0, bool = false);
    ~ProcessFile();
    bool isOutput() const;
    TypeFileItem get();
    long getAssignedProcess() const;
    void setAssignedProcess(long _processID);
    void setOutputState(bool);
};

typedef std::shared_ptr<ProcessFile> TypeProcessFile;
typedef std::vector<TypeProcessFile> TypeProcessFileList;

#endif //DICON_PROCESSFILE_H
