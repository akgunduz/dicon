//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_DEVICELIST_H
#define DICON_DEVICELIST_H

#include "Device.h"

typedef std::vector<Device*> TypeDeviceList;

class DeviceList {

    static DeviceList *instance;

    DeviceList();

    TypeDeviceList list;

public:

    static DeviceList* getInstance();
    bool add(Device*);
    Device* get(int);
    long getCount();

};

#endif //DICON_DEVICELIST_H
