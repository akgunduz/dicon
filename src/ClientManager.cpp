//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ClientManager.h"

ClientManager::ClientManager(Connector *connector,
		fTimeoutCB timeoutCB, fWakeupCB wakeupCB, double backupRate) {

	mClientConnector = connector;
	mTimeoutCB = timeoutCB;
	mWakeupCB = wakeupCB;
	mBackupRate = backupRate;

	LOG_I("ClientManager is created!!!");
};

ClientManager::~ClientManager() {

	stopClientChecker();
	pthread_join(mThreadClientChecker, nullptr);

	for (std::map<uint64_t, ClientMap*>::iterator i = mClients.begin(); i != mClients.end(); i++) {
		ClientMap* client = i->second;
		if (client->is_timer_active) {
			stopClientTimer(client);
			pthread_join(client->mThread, nullptr);
		}
	}
}

bool ClientManager::initClientTimer(ClientMap *clientMap, uint64_t collectorAddress) {

	if (mClientConnector == nullptr) {
		LOG_I("Client Connector is not initiated");
		return false;
	}

	if (clientMap->is_timer_active) {
		LOG_I("Client Timer is already active");
		return false;
	}

	clientMap->is_timer_active = true;
	int res = pthread_mutex_init(&clientMap->mMutex, NULL);
	if (res) {
		LOG_E("Mutex init failed");
		return false;
	}

	res = pthread_cond_init(&clientMap->mCond, NULL);
	if (res) {
		LOG_E("Condition init fail");
		pthread_mutex_destroy(&clientMap->mMutex);
		return false;
	}

	struct ClientManagerArgument *argument =
			new ClientManagerArgument(mClientConnector,
					mTimeoutCB, clientMap, collectorAddress);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int pthr = pthread_create(&clientMap->mThread, &attr, runClientTimer, (void *)argument);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run thread");
		pthread_cond_destroy(&clientMap->mCond);
		pthread_mutex_destroy(&clientMap->mMutex);
		clientMap->is_timer_active = false;
		return false;
	}
	return true;

}

bool ClientManager::stopClientTimer(ClientMap *clientMap) {

	pthread_cond_signal(&clientMap->mCond);
	return true;
}

void *ClientManager::runClientTimer(void *arg) {

	struct timespec timer;
	struct timeval now;

	gettimeofday(&now, NULL);

	timer.tv_sec = now.tv_sec + CLIENT_TIMEOUT;
	timer.tv_nsec = 0;

	struct ClientManagerArgument *argument = (ClientManagerArgument *) arg;

	pthread_mutex_lock(&argument->clientMap->mMutex);
	int res = pthread_cond_timedwait(&argument->clientMap->mCond,
			&argument->clientMap->mMutex, &timer);
	if (res == ETIMEDOUT) {
		LOG_E("Could not get a response for a specific time");
		argument->timeoutCB(argument->clientConnector,
				argument->clientMap->address, argument->collectorAddress);

	}

	pthread_mutex_unlock(&argument->clientMap->mMutex);

	pthread_cond_destroy(&argument->clientMap->mCond);
	pthread_mutex_destroy(&argument->clientMap->mMutex);

	argument->clientMap->is_timer_active = false;

	delete argument;

	return nullptr;
};

bool ClientManager::initClientChecker() {

	if (mClientConnector == nullptr) {
		LOG_I("Client Connector is not initiated");
		return false;
	}

	int res = pthread_mutex_init(&mMutexClientChecker, NULL);
	if (res) {
		LOG_E("mMutexClientChecker init failed");
		return false;
	}

	res = pthread_cond_init (&mCondClientChecker, NULL);
	if (res) {
		LOG_E("mCondClientChecker init fail");
		pthread_mutex_destroy(&mMutexClientChecker);
		return false;
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int pthr = pthread_create(&mThreadClientChecker, &attr, runClientChecker, this);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run mThreadClientChecker");
		pthread_cond_destroy(&mCondClientChecker);
		pthread_mutex_destroy(&mMutexClientChecker);
		return false;
	}
	return true;
}

void *ClientManager::runClientChecker(void *arg) {

	struct timespec timer;
	struct timeval now;

	struct ClientManager *clientManager = (ClientManager *) arg;

	while(1) {

		gettimeofday(&now, NULL);

		timer.tv_sec = now.tv_sec + CHECK_INTERVAL;
		timer.tv_nsec = 0;

		pthread_mutex_lock(&clientManager->mMutexClientChecker);
		int res = pthread_cond_timedwait(&clientManager->mCondClientChecker,
				&clientManager->mMutexClientChecker, &timer);
		if (res == ETIMEDOUT) {
			LOG_I("Sending Wakeupssss!!!");
			clientManager->mWakeupCB(clientManager->mClientConnector);
		} else {
			break;
		}

		pthread_mutex_unlock(&clientManager->mMutexClientChecker);

	}
	pthread_cond_destroy(&clientManager->mCondClientChecker);
	pthread_mutex_destroy(&clientManager->mMutexClientChecker);
	LOG_I("Ending Client Checking!!!");

	return nullptr;
};

bool ClientManager::stopClientChecker() {

	pthread_cond_signal(&mCondClientChecker);
	return true;
}

bool ClientManager::setClientIdle(uint64_t address, double totalTime) {

	try {

		ClientMap *clientMap = mClients.at(address);
		clientMap->state = IDLE;
		LOG_T("Client at address : %s switch to state : %s", Tools::getAddressStr(address).c_str(), sStates[IDLE]);

		if (clientMap->diffTime.isInitiated()) {
			LOG_U(UI_UPDATE_DIST_LOG,
					"Client at address : %s finished job in %.3lf seconds, total time passed : %.3lf",
					Tools::getAddressStr(address).c_str(), clientMap->diffTime.stop(), totalTime);
		}

	} catch (const std::out_of_range e) {

		addClient(address);
		return false;
	}

	return true;

}

bool ClientManager::setClientValidate(uint64_t address) {

	std::map<uint64_t, ClientMap*>::iterator i = mClients.find(address);
	if (i == mClients.end()) {
		addClient(address);
		return false;
	}

	LOG_T("Client at address : %s is Alive", Tools::getAddressStr(address).c_str());
	return true;

}

bool ClientManager::setClientBusy(uint64_t address) {

	try {

		ClientMap *clientMap = mClients.at(address);
		stopClientTimer(clientMap);
		clientMap->state = BUSY;
		clientMap->usage++;
		clientMap->diffTime.start();
		LOG_T("Client at address : %s switch to state : %s", Tools::getAddressStr(address).c_str(), sStates[BUSY]);

	} catch (const std::out_of_range e) {
		LOG_E("Could not found a client with address : %s", Tools::getAddressStr(address).c_str());
		return false;
	}

	return true;

}

bool ClientManager::setClientRemove(uint64_t address) {

	try {

		ClientMap *clientMap = mClients.at(address);
		mClients.erase(address);
		delete clientMap;

		mReadyBackup = (uint32_t) fmin(mTotalBackup, mReadyBackup + 1);

		LOG_U(UI_UPDATE_DIST_BACKUP, mTotalBackup, mReadyBackup);
		LOG_T("Client at address %s removed from the list", Tools::getAddressStr(address).c_str());

	} catch (const std::out_of_range e) {
		LOG_E("Could not found a client with address : %s", Tools::getAddressStr(address).c_str());
		return false;
	}

	return true;

}

bool ClientManager::addClient(uint64_t address) {

	mClients[address] = new ClientMap(IDLE, 0, address);

	mTotalBackup = mClients.size() == 1 ? 0 : (uint32_t) ceil(mClients.size() * mBackupRate);

	mReadyBackup = 0;

	LOG_U(UI_UPDATE_DIST_BACKUP, mTotalBackup, mReadyBackup);
	LOG_T("Client at address : %s added to the list", Tools::getAddressStr(address).c_str());

	return true;
}

uint64_t ClientManager::getIdleClient(uint64_t collectorAddress) {

	ClientMap *leastUsedClient = nullptr;

	int idleCount = 0;

	for (std::map<uint64_t, ClientMap*>::iterator i = mClients.begin(); i != mClients.end(); i++) {
		ClientMap *clientMap = i->second;
		if (clientMap->state != IDLE) {
			continue;
		}

		idleCount++;

		if (leastUsedClient == nullptr) {
			leastUsedClient = clientMap;
			continue;
		}

		if (clientMap->usage < leastUsedClient->usage) {
			leastUsedClient = clientMap;
		}
	}

	if (leastUsedClient != nullptr && idleCount - mTotalBackup + mReadyBackup > 0) {
		leastUsedClient->state = PREBUSY;

		initClientTimer(leastUsedClient, collectorAddress);

		LOG_T("Client at address : %s returned to collector",
				Tools::getAddressStr(leastUsedClient->address).c_str());

		return leastUsedClient->address;

	} else {
		LOG_W("No available client right now.");
	}

	return 0;
}

bool ClientManager::resetDiffTimes() {

	for (std::map<uint64_t, ClientMap*>::iterator i = mClients.begin(); i != mClients.end(); i++) {
		i->second->diffTime.reset();
	}
	return true;
}

void ClientManager::clear() {

	mClients.clear();
	mReadyBackup = 0;
	mTotalBackup = 0;
}
