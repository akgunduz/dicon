//
// Created by Haluk Akgunduz on 01/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Connector.h"
#include "Net.h"
#include "Pipe.h"
#include "Tools.h"
#include "UnixSocket.h"

Connector::Connector(uint32_t interfaceIndex, const CallBack *cb, const std::string &rootPath) {

	INTERFACES  interfaceType = ConnInterface::getInterfaceType(interfaceIndex);

	try {

		switch(interfaceType) {

			case INTERFACE_NET:
				mInterface = new Net(interfaceIndex, cb, rootPath);
				break;

			case INTERFACE_PIPE:
				mInterface = new Pipe(interfaceIndex, cb, rootPath);
				break;

			case INTERFACE_UNIXSOCKET:
				mInterface = new UnixSocket(interfaceIndex, cb, rootPath);
				break;

			default:
				break;
		}

	} catch (const std::runtime_error e) {

		LOG_E("Interface Init failed!!!");
		throw std::runtime_error("Connector : Interface Init failed!!!");
	}

	mInitialized = true;
}

bool Connector::send(uint64_t target, Message *msg) {

	if (target == 0) {
		LOG_E("Invalid target : %d", target);
		return false;
	}

	INTERFACES interface = Tools::getInterface(target);

	return mInterface->push((int)MESSAGE_SEND + (uint16_t)interface, target, msg);

}

uint64_t Connector::getAddress() {

	if (!mInitialized) {
		return 0;
	}

	return mInterface->getAddress();

}

std::vector<uint64_t> Connector::getAddressList() {

	if (!mInitialized) {

		std::vector<uint64_t> list;
		return list;
	}

	return mInterface->getAddressList();

}

int Connector::getNotifier(NOTIFIER_TYPE type) {

	if (!mInitialized) {
		return 0;
	}

	return mInterface->getNotifier(type);

}

Connector::~Connector() {

	delete mInterface;
	mInterface = nullptr;

}


INTERFACES Connector::getInterfaceType() {

	return mInterface->getType();

}

void Connector::setRootPath(std::string &path) {

	mInterface->mRootPath = path;

}

std::string Connector::getRootPath() {

	return mInterface->mRootPath;
}

Interface *Connector::getInterface() {

	return mInterface;

}
