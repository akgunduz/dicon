//
// Created by Haluk AKGUNDUZ on 03/04/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_DEVICELIST_H
#define DICON_DEVICELIST_H

#include "Device.h"

class DeviceList {

    static inline DeviceList *instance = nullptr;

    DeviceList();

    TypeDeviceList list;

public:

    ~DeviceList();
    static DeviceList* getInstance();
    bool add(TypeDevice);
    TypeDevice& get(int);
    size_t getCount();

};

#endif //DICON_DEVICELIST_H
