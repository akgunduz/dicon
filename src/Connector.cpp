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
				interface = new Net(interfaceIndex, cb, rootPath);
				break;

			case INTERFACE_PIPE:
				interface = new Pipe(interfaceIndex, cb, rootPath);
				break;

			case INTERFACE_UNIXSOCKET:
				interface = new UnixSocket(interfaceIndex, cb, rootPath);
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

bool Connector::send(Address* target, Message *msg) {

	if (!initialized) {
		return false;
	}

	return interface->push(MESSAGE_SEND, target, msg);

}

Address* Connector::getAddress() {

	if (!initialized) {
		return 0;
	}

	return interface->getAddress();

}

std::vector<long> Connector::getAddressList() {

	if (!initialized) {

		std::vector<long> list;
		return list;
	}

	return interface->getAddressList();

}

int Connector::getNotifier(NOTIFIER_TYPE type) {

	if (!initialized) {
		return 0;
	}

	return interface->getNotifier(type);

}

Connector::~Connector() {

	delete interface;
	interface = nullptr;

}


INTERFACES Connector::getInterfaceType() {

	return interface->getType();

}

void Connector::setRootPath(std::string &path) {

	interface->rootPath = path;

}

std::string Connector::getRootPath() {

	return interface->rootPath;
}

Interface *Connector::getInterface() {

	return interface;

}
