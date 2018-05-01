//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_USERINTERFACEEVENT_H
#define BANKOR_USERINTERFACEEVENT_H

#include "Common.h"

class UserInterfaceEvent {

    int id;
    std::vector<long> data;
    std::vector<std::string> str;
    std::vector<void*> pointer;

public:

    UserInterfaceEvent(int);

    long getData(int);
    long getDataSize();
    std::string getString(int);
    void* getPointer(int);

    void setID(int);
    void addData(long);
    void addDataList(std::vector<long>);
    void addString(std::string);
    void addPointer(void*);
};


#endif //BANKOR_USERINTERFACEEVENT_H
