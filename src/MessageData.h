//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_MESSAGEDATA_H
#define BANKOR_MESSAGEDATA_H

#include "ExecutorItem.h"
#include "Uuid.h"

#define STREAM_NONE 0xFFFF

#define STREAM_INFO 0x01
#define STREAM_BINARY 0x02
#define STREAM_MD5 0x03
#define STREAM_JOB 0x04
#define STREAM_JOBID 0x05

class MessageData {

    int streamFlag;

    long executorID;
    char executor[PATH_MAX];
    Uuid jobID;
    char jobDir[PATH_MAX];

    TypeMD5List md5List;
    TypeFileInfoList fileList;

public:

    MessageData();
    MessageData(MessageData &);
    MessageData& operator=(MessageData*);

    int getStreamFlag();
    MessageData& setStreamFlag(int);

    Md5* getMD5(int);
    TypeMD5List* getMD5List();
    MessageData& addMD5(Md5);
    MessageData& addMD5List(TypeMD5List*);
    unsigned long getMD5Count();

    FileItem* getFile(int);
    bool isOutput(int);
    TypeFileInfoList* getFileList();
    MessageData& addFile(FileInfo);
    MessageData& addFileList(TypeFileInfoList*);
    unsigned long getFileCount();

    char* getExecutor();
    long getExecutorID();
    MessageData& setExecutorID(long);
    MessageData& setExecutor(long, const char*);

    Uuid getJobID();
    char* getJobDir();
    MessageData& setJob(Uuid, const char*);
    MessageData& setJobID(Uuid);

};


#endif //BANKOR_MESSAGEDATA_H
