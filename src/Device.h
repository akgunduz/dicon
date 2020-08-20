//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef DICON_DEVICE_H
#define DICON_DEVICE_H

#include "InterfaceTypes.h"

class Device {

    std::string name;

    INTERFACE type;

    long base;
    int mask;
    bool loopback;

public:

    Device(std::string , INTERFACE, uint32_t , uint8_t = 0, bool = false);
    ~Device();

    const std::string& getName() const;
    INTERFACE getType() const;
    uint32_t getBase() const;
    uint8_t getMask() const;
    bool isLoopback() const;

};

typedef std::unique_ptr<Device> TypeDevice;
typedef std::vector<TypeDevice> TypeDeviceList;

#endif //DICON_DEVICE_H
