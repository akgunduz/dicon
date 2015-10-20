//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "Pipe.h"
#include "UnixSocket.h"

Connector::Connector(Unit host, Device *device, bool multicastEnabled,
                     const InterfaceCallback *cb, const char *rootPath) {

	INTERFACES  interfaceType = device->getType();

	try {

		switch(interfaceType) {

			case INTERFACE_NET:
				_interface = new Net(host, device, multicastEnabled, cb, rootPath);
				break;

			case INTERFACE_PIPE:
				_interface = new Pipe(host, device, false, cb, rootPath);
				break;

			case INTERFACE_UNIXSOCKET:
				_interface = new UnixSocket(host, device, false, cb, rootPath);
				break;

			default:
				break;
		}

	} catch (const std::runtime_error e) {

		LOG_E("Interface Init failed!!!");
		throw std::runtime_error("Connector : Interface Init failed!!!");
	}

	initialized = true;
}

bool Connector::send(long target, Message *msg) {

	if (!initialized) {
		return false;
	}

	return _interface->push(MESSAGE_SEND, target, msg);

}

bool Connector::send(Message *msg) {

    if (!initialized) {
        return false;
    }

    return _interface->push(MESSAGE_SEND, _interface->getMulticastAddress(), msg);

}

bool Connector::put(long target, Message *msg) {

    if (!initialized) {
        return false;
    }

    return _interface->push(MESSAGE_RECEIVE, target, msg);

}

long Connector::getAddress() {

	if (!initialized) {
		return 0;
	}

	return _interface->getAddress();

}

std::vector<long> Connector::getAddressList() {

	if (!initialized) {

		std::vector<long> list;
		return list;
	}

	return _interface->getAddressList();

}

Connector::~Connector() {

	delete _interface;
	_interface = nullptr;

}

INTERFACES Connector::getInterfaceType() {

	return _interface->getType();

}

const char *Connector::getRootPath() {

	return _interface->getRootPath();
}

Interface *Connector::getInterface() {

	return _interface;

}

Device* Connector::getDevice() {
    return _interface->getDevice();
}
