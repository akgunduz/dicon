//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "UnixSocket.h"

std::vector<Device> Connector::deviceList;
Device* Connector::selectedDevices[2];
bool Connector::devicesInitialized = false;

Interface* Connector::createInterface(const Unit component, Device *device, const InterfaceCallback *cb) {

    Interface *interface = NULL;

    switch(device->getType()) {

        case INTERFACE_NET:
            interface = new Net(component, device, cb);
            break;

        case INTERFACE_UNIXSOCKET:
            interface = new UnixSocket(component, device, cb);
            break;

        default:
            break;
    }

    return interface;
}

bool Connector::createDevices() {

    std::vector<Device> *netDevices = Net::getDevices();
    deviceList.insert(deviceList.end(), netDevices->begin(), netDevices->end());

    std::vector<Device> *unixSocketDevices = UnixSocket::getDevices();
    deviceList.insert(deviceList.end(), unixSocketDevices->begin(), unixSocketDevices->end());

    return true;
}

unsigned long Connector::getCount() {

    return getDevices()->size();

}

std::vector<Device> *Connector::getDevices() {

    if (deviceList.empty()) {
        createDevices();
    }
    return &deviceList;
}

Device* Connector::getDevice(int index) {

    return &deviceList[index >= deviceList.size() ?
                                      deviceList.size() - 1 : index];
}

void Connector::setSelectedDevices(int other, int node) {

	if (!devicesInitialized) {
		selectedDevices[0] = getDevice(other);
		selectedDevices[1] = getDevice(node);
		devicesInitialized = true;
	}
}

Device* Connector::getSelectedDevice(unsigned char index) {

    return selectedDevices[index];
}
