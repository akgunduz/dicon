//
// Created by Haluk Akgunduz on 11/14/15.
//

#ifndef BANKOR_DEVICEMANAGER_H
#define BANKOR_DEVICEMANAGER_H


#include "Device.h"

class DeviceManager {

    static DeviceManager *instance;

    DeviceManager();

    int lastFreePort;

    std::vector<Device>deviceList;

    bool createDevices();

    static Device* getFreeDeviceTCP();
    static Device* getFreeDeviceLoopback();
    static Device* getFreeDeviceUnixSocket();

public:

    static DeviceManager* getInstance();
    static std::vector<Device>* getDevices();
    static Device* getDevice(unsigned long);
    static unsigned long getCount();
    //static Device* getFreeDevice(CONNECTTYPE);
};


#endif //BANKOR_DEVICEMANAGER_H
