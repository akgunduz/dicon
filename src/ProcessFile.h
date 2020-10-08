//
// Created by Haluk AKGUNDUZ on 22.07.2020.
//

#ifndef DICON_PROCESSFILE_H
#define DICON_PROCESSFILE_H

#include "FileItem.h"

enum PROCESS_FILE_OPTIONS {
    PROCESS_FILE_EXEC,
    PROCESS_FILE_INPUT,
    PROCESS_FILE_OUTPUT,
    PROCESS_FILE_MAX
};

class ProcessFile {

    PROCESS_FILE_OPTIONS fileType {PROCESS_FILE_INPUT};

    long processID{};

    TypeFileItem content;

public:

    explicit ProcessFile(TypeFileItem, long = 0, PROCESS_FILE_OPTIONS = PROCESS_FILE_INPUT);
    ~ProcessFile() = default;
    bool isOutput() const;
    bool isExecutable() const;
    TypeFileItem get();
    long getAssignedProcess() const;
    void setAssignedProcess(long _processID);
    void setOutputState(bool);
};

typedef std::shared_ptr<ProcessFile> TypeProcessFile;
typedef std::vector<TypeProcessFile> TypeProcessFileList;

#endif //DICON_PROCESSFILE_H
