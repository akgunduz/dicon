//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_MESSAGEDATA_H
#define BANKOR_MESSAGEDATA_H

#include "ExecutorItem.h"
#include "Uuid.h"

#define FLAG_NONE           0x00000000
#define FLAG_JOBID          0x00000001
#define FLAG_JOBDIR         0x00000002
#define FLAG_PROCESSID      0x00000004
#define FLAG_PROCESSCMD     0x00000008
#define FLAG_FILEINFO       0x00000010
#define FLAG_FILEBINARY     0x00000020
#define FLAG_FILEMD5        0x00000040

#define FLAG_JOB            FLAG_JOBID | FLAG_JOBDIR
#define FLAG_PROCESS        FLAG_PROCESSID | FLAG_PROCESSCMD
#define FLAG_FILE           FLAG_FILEINFO | FLAG_FILEBINARY


class MessageData {

    unsigned long streamFlag;

    long processID;
    char processCommand[PATH_MAX];
    Uuid jobID;
    char jobDir[PATH_MAX];

    TypeMD5List md5List;
    TypeFileInfoList fileList;

public:

    MessageData();
    MessageData(MessageData &);
    MessageData& operator=(MessageData*);

    unsigned long getStreamFlag();
    MessageData& setStreamMask(unsigned long);
    MessageData& setStreamFlag(unsigned long);

    Md5* getMD5(int);
    TypeMD5List* getMD5List();
    MessageData& addMD5(Md5);
    MessageData& addMD5List(TypeMD5List*);
    unsigned long getMD5Count();

    FileItem* getFile(int);
    bool isOutput(int);
    TypeFileInfoList* getFileList();
    MessageData& addFile(FileInfo, bool);
    MessageData& addFileList(TypeFileInfoList*, bool);
    unsigned long getFileCount();

    char* getProcessCommand();
    long getProcessID();
    MessageData& setProcessID(long);
    MessageData& setProcess(long, const char *);

    Uuid getJobID();
    char* getJobDir();
    MessageData& setJob(Uuid, const char*);
    MessageData& setJobID(Uuid);

};


#endif //BANKOR_MESSAGEDATA_H
