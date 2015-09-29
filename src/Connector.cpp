//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "Pipe.h"
#include "UnixSocket.h"

Connector::Connector(uint32_t interfaceIndex, const InterfaceCallback *cb, const std::string &rootPath) {

	INTERFACES  interfaceType = ConnInterface::getInterfaceType(interfaceIndex);

	try {

		switch(interfaceType) {

			case INTERFACE_NET:
				_interface = new Net(interfaceIndex, cb, rootPath);
				break;

			case INTERFACE_PIPE:
				_interface = new Pipe(interfaceIndex, cb, rootPath);
				break;

			case INTERFACE_UNIXSOCKET:
				_interface = new UnixSocket(interfaceIndex, cb, rootPath);
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

int Connector::getNotifier(NOTIFIER_TYPE type) {

	if (!initialized) {
		return 0;
	}

	return _interface->getNotifier(type);

}

Connector::~Connector() {

	delete _interface;
	_interface = nullptr;

}


INTERFACES Connector::getInterfaceType() {

	return _interface->getType();

}

void Connector::setRootPath(std::string &path) {

	_interface->rootPath = path;

}

std::string Connector::getRootPath() {

	return _interface->rootPath;
}

Interface *Connector::getInterface() {

	return _interface;

}
