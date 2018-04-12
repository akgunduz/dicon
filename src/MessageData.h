//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_MESSAGEDATA_H
#define BANKOR_MESSAGEDATA_H


#include "ExecutorItem.h"

class MessageData {

    void *variant[3];
//
//
//
    ExecutorItem *executorItem;
//
//    int executionID;
//
//    int jobID;

    int streamFlag;

//    char jobDir[50];

    TypeMD5List md5List;
    TypeFileList fileList;
    char executor[PATH_MAX];
    char jobDir[PATH_MAX];

public:

    int getStreamFlag();
    void setStreamFlag(int);
   // char* getJobDir();

 //   void setJob(int);

    Md5* getMD5(int);
    TypeMD5List* getMD5List();
    void addMD5(Md5);
    void addMD5List(TypeMD5List*);
    unsigned long getMD5Count();


    FileItem* getFile(int);
    TypeFileList* getFileList();
    void addFile(FileItem*);
    void addFileList(TypeFileList*);
    unsigned long getFileCount();

    char* getExecutor();
    void setExecutor(char*);

    char* getJobDir();
    void setJobDir(char*);
};


#endif //BANKOR_MESSAGEDATA_H
