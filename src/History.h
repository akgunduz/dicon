//
// Created by Haluk AKGUNDUZ on 15/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_HISTORY_H
#define DICON_HISTORY_H


#include "Md5.h"

typedef std::map<const char*, std::vector<Md5> > TypeJobMD5List;

class History {

private:

    TypeJobMD5List list;
public:

    bool loadHistory(const char*);
    bool saveHistory(const char*);
    bool addHistory(const char*, Md5);
    bool removeHistory(const char*, Md5);
    bool checkHistory(const char*, Md5);
};


#endif //DICON_HISTORY_H
