//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef DICON_DEVICE_H
#define DICON_DEVICE_H

#include <string>
#include <memory>
#include <vector>
#include "CommInterfaceType.h"

#define FAKE_US_ADDRESS 0xFFFFFFFF
#define FAKE_US_MASK 24

class Device {

    std::string name;

    COMM_INTERFACE type;

    long base;
    int mask;
    bool loopback;

public:

    Device(std::string , COMM_INTERFACE, uint32_t , uint8_t = 0, bool = false);
    ~Device();

    const std::string& getName() const;
    COMM_INTERFACE getType() const;
    uint32_t getBase() const;
    uint8_t getMask() const;
    bool isLoopback() const;

};

typedef std::shared_ptr<Device> TypeDevice;
typedef std::vector<TypeDevice> TypeDeviceList;

#endif //DICON_DEVICE_H
