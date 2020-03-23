//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_DEVICELIST_H
#define DICON_DEVICELIST_H

#include "Device.h"

#define MAX_ACTIVE 2

class DeviceList {

private:

    std::vector<Device*> devices;
    int active[MAX_ACTIVE];

    static DeviceList *instance;
    DeviceList();

public:

    static DeviceList* getInstance();
    bool add(Device*);
    Device* get(int);
    Device* getActive(int);
    long getCount();
    void setActive(int, int);
    bool isActiveDifferent();
};


#endif //DICON_DEVICELIST_H
