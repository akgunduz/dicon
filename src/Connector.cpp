//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "UnixSocket.h"

std::vector<Device> Connector::deviceList;
Device* Connector::selectedDevices[2];

Connector::Connector(const Unit component, Device* device, const InterfaceCallback *cb) {

	try {

		switch(device->getType()) {

			case INTERFACE_NET:
				_interface = new Net(component, device, cb);
				break;

			case INTERFACE_UNIXSOCKET:
				_interface = new UnixSocket(component, device, cb);
				break;

			default:
				break;
		}

	} catch (const std::runtime_error e) {

		LOG_E("Interface Init failed!!!");
		throw std::runtime_error("Connector : Interface Init failed!!!");
	}
}

Connector::~Connector() {

    delete _interface;
}

bool Connector::send(long target, Message *msg) {

	return _interface->push(MESSAGE_SEND, target, msg);
}

bool Connector::send(Message *msg) {

    return _interface->push(MESSAGE_SEND, _interface->getMulticastAddress(), msg);
}

bool Connector::put(long target, Message *msg) {

    return _interface->push(MESSAGE_RECEIVE, target, msg);
}

long Connector::getAddress() {

	return _interface->getAddress();
}

std::vector<long> Connector::getAddressList() {

	return _interface->getAddressList();
}


INTERFACES Connector::getInterfaceType() {

	return _interface->getType();

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

    selectedDevices[0] = getDevice(other);
    selectedDevices[1] = getDevice(node);
}

Device* Connector::getSelectedDevice(unsigned char index) {

    return selectedDevices[index];
}