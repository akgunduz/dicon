//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"

Distributor::Distributor(uint32_t collInterfaceIndex,
		uint32_t clientInterfaceIndex, const std::string &rootPath, double backupRate) {

	mCallback = new CallBack(receiveCB, this);

	try {

		mCollectorConnector = new Connector(collInterfaceIndex, mCallback, rootPath);

	} catch (const std::runtime_error e) {

		LOG_E("Connector Init failed!!!");

		delete mCallback;

		throw std::runtime_error("Distributor : Connector Init failed!!!");
	}

	if (collInterfaceIndex != clientInterfaceIndex) {

		try {

			mClientConnector = new Connector(clientInterfaceIndex, mCallback, rootPath);

		} catch (const std::runtime_error e) {

			LOG_E("Connector Init failed!!!");

			delete mCollectorConnector;
			delete mCallback;

			throw std::runtime_error("Distributor : Connector Init failed!!!");
		}

	} else {

		mClientConnector = mCollectorConnector;

	}

	LOG_U(UI_UPDATE_DIST_ADDRESS, getAddress(HOST_COLLECTOR), getAddress(HOST_CLIENT));

	mRootPath = rootPath;

	mClientManager = new ClientManager(mClientConnector,
			timeoutCallback, sendWakeupMessage, backupRate);
	mClientManager->initClientChecker();

	LOG_I("Instance is created!!!");
};

Distributor::~Distributor() {

	delete mClientManager;

	if (mCollectorConnector != mClientConnector) {
		delete mClientConnector;
	}

	delete mCollectorConnector;

	delete mCallback;
}

bool Distributor::receiveCB(void *arg, uint64_t address, Message *msg) {

	Distributor *dist = (Distributor *) arg;

	switch(msg->getOwner()) {

		case HOST_CLIENT:
			if (dist->mClientConnector->getInterfaceType() == Tools::getInterface(address)) {
				dist->processClientMsg(address, msg);
			}
			break;

		case HOST_COLLECTOR:
			if (dist->mCollectorConnector->getInterfaceType() == Tools::getInterface(address)) {
				dist->processCollectorMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received : %d from %s, disgarding", msg->getType(), Tools::getAddressStr(address).c_str());
			delete msg;
			break;
	}

	return true;
}

INTERFACES Distributor::getInterfaceType(HOST host) {

	if (host == HOST_COLLECTOR) {
		return mCollectorConnector->getInterfaceType();
	} else {
		return mClientConnector->getInterfaceType();
	}

}

uint64_t Distributor::getAddress(HOST host) {

	if (host == HOST_COLLECTOR) {
		return mCollectorConnector->getAddress();
	} else {
		return mClientConnector->getAddress();
	}

}

bool Distributor::processCollectorMsg(uint64_t address, Message *msg) {

	bool status = false;
	msg->display();

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from collector: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_CLIENT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"CLIENT\" msg from collector: %s",
					Tools::getAddressStr(address).c_str());

			status = send2CollectorMsg(address, MSGTYPE_CLIENT);
			break;

		case MSGTYPE_TIME:
			mCollStartTime.start();
			mClientManager->resetDiffTimes();
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIME\" msg from collector: %s",
					Tools::getAddressStr(address).c_str());
			break;

		default:
			break;
	}

	delete msg;
	return status;

}

bool Distributor::processClientMsg(uint64_t address, Message *msg) {

	bool status = false;
	msg->display();

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from client: %s",
					Tools::getAddressStr(address).c_str());

			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, IDLE);

			mClientManager->setClientIdle(address, mCollStartTime.stop());

			if (mCollectorWaitingList.size() > 0) {
				uint64_t collAddress = mCollectorWaitingList.front();
				mCollectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
						"Processing a collector from the waiting list: %s",
						Tools::getAddressStr(collAddress).c_str());

				status = send2CollectorMsg(collAddress, MSGTYPE_CLIENT);

			} else {
				status = true;
			}

			break;

		case MSGTYPE_ALIVE:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"ALIVE\" msg from client: %s",
					Tools::getAddressStr(address).c_str());

			if (!mClientManager->setClientValidate(address)
					&& mCollectorWaitingList.size() > 0) {
				uint64_t collAddress = mCollectorWaitingList.front();
				mCollectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
						"Processing a collector from the waiting list: %s",
						Tools::getAddressStr(collAddress).c_str());

				status = send2CollectorMsg(collAddress, MSGTYPE_CLIENT);
			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, IDLE);

			break;

		case MSGTYPE_BUSY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"BUSY\" msg from client: %s",
					Tools::getAddressStr(address).c_str());

			mClientManager->setClientBusy(address);
			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, BUSY);


			status = true;
			break;

		case MSGTYPE_TIMEOUT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIMEOUT\" msg from client: %s",
					Tools::getAddressStr(address).c_str());

			mClientManager->setClientRemove(address);
			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, REMOVE);

			status = send2CollectorMsg(msg->getVariant(), MSGTYPE_CLIENT);

			break;

		default :
			break;
	}

	delete msg;
	return status;

}

bool Distributor::send2ClientMsg(uint64_t address, uint8_t type) {

	Message *msg = new Message(mRootPath);
	msg->setOwner(HOST_DISTRIBUTOR);
	msg->setType(type);

	switch(type) {

		case MSGTYPE_WAKEUP:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to client: %s",
					Tools::getAddressStr(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return mClientConnector->send(address, msg);

}

bool Distributor::send2CollectorMsg(uint64_t address, uint8_t type) {

	Message *msg = new Message(mRootPath);
	msg->setOwner(HOST_DISTRIBUTOR);
	msg->setType(type);

	switch(type) {

		case MSGTYPE_WAKEUP:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to collector: %s",
					Tools::getAddressStr(address).c_str());
			break;

		case MSGTYPE_CLIENT: {
				msg->setPriority(PRIORITY_2);
				uint64_t clientaddress = mClientManager->getIdleClient(address);
				if (clientaddress != 0) {

					LOG_U(UI_UPDATE_DIST_CLIENT_LIST, clientaddress, PREBUSY);
					LOG_U(UI_UPDATE_DIST_LOG,
							"\"CLIENT\" msg sent to collector: %s with available client: %s",
							Tools::getAddressStr(address).c_str(),
							Tools::getAddressStr(clientaddress).c_str());

				} else {
					mCollectorWaitingList.push_back(address);

					LOG_U(UI_UPDATE_DIST_LOG,
							"\"CLIENT\" msg sent to collector: %s with no available client",
							Tools::getAddressStr(address).c_str());

				}
				msg->setVariant(clientaddress);

				LOG_U(UI_UPDATE_DIST_COLL_LIST, address, clientaddress);

			}
			break;

		default:
			delete msg;
			return false;
	}

	return mCollectorConnector->send(address, msg);

}

std::string Distributor::getRootPath() {

	return mRootPath;

}

bool Distributor::sendWakeupMessagesAll() {

	sendWakeupMessage(mClientConnector);
	if (mCollectorConnector != mClientConnector) {
		sendWakeupMessage(mCollectorConnector);
	}
	return true;
}

bool Distributor::reset() {

	mClientManager->clear();
	mCollectorWaitingList.clear();
	mCollStartTime.reset();
	return true;

}

bool Distributor::timeoutCallback(Connector *connector,
		uint64_t address, uint64_t collectorAddress) {

	Message *msg = new Message(connector->getRootPath());
	msg->setOwner(HOST_CLIENT);
	msg->setTarget(HOST_DISTRIBUTOR);
	msg->setType(MSGTYPE_TIMEOUT);
	msg->setVariant(collectorAddress);
	connector->getInterface()->
			push(MESSAGE_RECEIVE, address, msg);

	return true;
}

bool Distributor::sendWakeupMessage(Connector *connector) {

	std::vector<uint64_t> list = connector->getAddressList();

	for (int i = 0; i < list.size(); i++) {

		Message *msg = new Message(connector->getRootPath());

		msg->setOwner(HOST_DISTRIBUTOR);
		msg->setType(MSGTYPE_WAKEUP);
		msg->setPriority(PRIORITY_1);

		connector->send(list[i], msg);

	}

	LOG_U(UI_UPDATE_DIST_LOG,
			"\"WAKEUP\" messages sent to network");

	return true;
}
