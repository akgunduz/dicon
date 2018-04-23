//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_MESSAGEDATA_H
#define BANKOR_MESSAGEDATA_H


#include "ExecutorItem.h"

class MessageData {

    int streamFlag;

    TypeMD5List md5List;
    TypeFileInfoList fileList;
    char executor[PATH_MAX];
    char jobDir[PATH_MAX];

public:

    int getStreamFlag();
    void setStreamFlag(int);

    Md5* getMD5(int);
    TypeMD5List* getMD5List();
    void addMD5(Md5);
    void addMD5List(TypeMD5List*);
    unsigned long getMD5Count();

    FileItem* getFile(int);
    uint8_t getState(int);
    TypeFileInfoList* getFileList();
    void addFile(FileInfo);
    void addFileList(TypeFileInfoList*);
    unsigned long getFileCount();

    char* getExecutor();
    void setExecutor(const char*);

    char* getJobDir();
    void setJobDir(const char*);
};


#endif //BANKOR_MESSAGEDATA_H
