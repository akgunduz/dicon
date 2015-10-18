//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"

NodeManager::NodeManager(Connector *connector,
		fTimeoutCB timeoutCB, fWakeupCB wakeupCB, double backupRate) {

	clientConnector = connector;
	this->timeoutCB = timeoutCB;
	this->wakeupCB = wakeupCB;
	this->backupRate = backupRate;

	LOG_I("NodeManager is created!!!");
};

NodeManager::~NodeManager() {

	stopClientChecker();
	pthread_join(threadClientChecker, nullptr);

	for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
		NodeItem * client = i->second;
		if (client->is_timer_active) {
			stopClientTimer(client);
			pthread_join(client->thread, nullptr);
		}
	}
}

bool NodeManager::initClientTimer(NodeItem *clientMap, long collectorAddress) {

	if (clientConnector == nullptr) {
		LOG_I("Node Connector is not initiated");
		return false;
	}

	if (clientMap->is_timer_active) {
		LOG_I("Node Timer is already active");
		return false;
	}

	clientMap->is_timer_active = true;
	int res = pthread_mutex_init(&clientMap->mutex, NULL);
	if (res) {
		LOG_E("Mutex init failed");
		return false;
	}

	res = pthread_cond_init(&clientMap->cond, NULL);
	if (res) {
		LOG_E("Condition init fail");
		pthread_mutex_destroy(&clientMap->mutex);
		return false;
	}

	struct ClientManagerArgument *argument =
			new ClientManagerArgument(clientConnector,
									  timeoutCB, clientMap, collectorAddress);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int pthr = pthread_create(&clientMap->thread, &attr, runClientTimer, (void *)argument);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run thread");
		pthread_cond_destroy(&clientMap->cond);
		pthread_mutex_destroy(&clientMap->mutex);
		clientMap->is_timer_active = false;
		return false;
	}
	return true;

}

bool NodeManager::stopClientTimer(NodeItem *clientMap) {

	pthread_cond_signal(&clientMap->cond);
	return true;
}

void *NodeManager::runClientTimer(void *arg) {

	struct timespec timer;
	struct timeval now;

	gettimeofday(&now, NULL);

	timer.tv_sec = now.tv_sec + CLIENT_TIMEOUT;
	timer.tv_nsec = 0;

	struct ClientManagerArgument *argument = (ClientManagerArgument *) arg;

	pthread_mutex_lock(&argument->clientMap->mutex);
	int res = pthread_cond_timedwait(&argument->clientMap->cond,
			&argument->clientMap->mutex, &timer);
	if (res == ETIMEDOUT) {
		LOG_E("Could not get a response for a specific time");
		argument->timeoutCB(argument->clientConnector,
				argument->clientMap->address, argument->collectorAddress);

	}

	pthread_mutex_unlock(&argument->clientMap->mutex);

	pthread_cond_destroy(&argument->clientMap->cond);
	pthread_mutex_destroy(&argument->clientMap->mutex);

	argument->clientMap->is_timer_active = false;

	delete argument;

	return nullptr;
};

bool NodeManager::initClientChecker() {

	if (clientConnector == nullptr) {
		LOG_I("Node Connector is not initiated");
		return false;
	}

	int res = pthread_mutex_init(&mutexClientChecker, NULL);
	if (res) {
		LOG_E("mutexClientChecker init failed");
		return false;
	}

	res = pthread_cond_init (&condClientChecker, NULL);
	if (res) {
		LOG_E("condClientChecker init fail");
		pthread_mutex_destroy(&mutexClientChecker);
		return false;
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int pthr = pthread_create(&threadClientChecker, &attr, runClientChecker, this);
	pthread_attr_destroy(&attr);
	if (pthr) {
		LOG_E("Problem with run threadClientChecker");
		pthread_cond_destroy(&condClientChecker);
		pthread_mutex_destroy(&mutexClientChecker);
		return false;
	}
	return true;
}

void *NodeManager::runClientChecker(void *arg) {

	struct timespec timer;
	struct timeval now;

	struct NodeManager *clientManager = (NodeManager *) arg;

	while(1) {

		gettimeofday(&now, NULL);

		timer.tv_sec = now.tv_sec + CHECK_INTERVAL;
		timer.tv_nsec = 0;

		pthread_mutex_lock(&clientManager->mutexClientChecker);
		int res = pthread_cond_timedwait(&clientManager->condClientChecker,
				&clientManager->mutexClientChecker, &timer);
		if (res == ETIMEDOUT) {
			LOG_I("Sending Wakeupssss!!!");
		//	clientManager->wakeupCB(clientManager->clientConnector);
		} else {
			break;
		}

		pthread_mutex_unlock(&clientManager->mutexClientChecker);

	}
	pthread_cond_destroy(&clientManager->condClientChecker);
	pthread_mutex_destroy(&clientManager->mutexClientChecker);
	LOG_I("Ending Node Checking!!!");

	return nullptr;
};

bool NodeManager::stopClientChecker() {

	pthread_cond_signal(&condClientChecker);
	return true;
}

bool NodeManager::setClientIdle(long address, short id, double totalTime) {

	try {

		NodeItem *clientMap = nodes.at(address);
		clientMap->state = IDLE;
		LOG_T("Node at address : %s switch to state : %s", Address::getString(address).c_str(), sStates[IDLE]);

		if (clientMap->stopWatch.isInitiated()) {
			LOG_U(UI_UPDATE_DIST_LOG,
					"Node at address : %s finished job in %.3lf seconds, total time passed : %.3lf",
				  Address::getString(address).c_str(), clientMap->stopWatch.stop(), totalTime);
		}

	} catch (const std::out_of_range e) {

		addClient(address, id);
		return false;
	}

	return true;

}

bool NodeManager::setClientValidate(long address, short id) {

	std::map<long, NodeItem *>::iterator i = nodes.find(address);
	if (i == nodes.end()) {
		addClient(address, id);
		return false;
	}

	LOG_T("Node at address : %s is Alive", Address::getString(address).c_str());
	return true;

}

bool NodeManager::setClientBusy(long address) {

	try {

		NodeItem *clientMap = nodes.at(address);
		stopClientTimer(clientMap);
		clientMap->state = BUSY;
		clientMap->usage++;
		clientMap->stopWatch.start();
		LOG_T("Node at address : %s switch to state : %s", Address::getString(address).c_str(), sStates[BUSY]);

	} catch (const std::out_of_range e) {
		LOG_E("Could not found a client with address : %s", Address::getString(address).c_str());
		return false;
	}

	return true;

}

bool NodeManager::setClientRemove(long address) {

	try {

		NodeItem *clientMap = nodes.at(address);
		nodes.erase(address);
		delete clientMap;

		readyBackup = (uint32_t) fmin(totalBackup, readyBackup + 1);

		LOG_U(UI_UPDATE_DIST_BACKUP, totalBackup, readyBackup);
		LOG_T("Node at address %s removed from the list", Address::getString(address).c_str());

	} catch (const std::out_of_range e) {
		LOG_E("Could not found a client with address : %s", Address::getString(address).c_str());
		return false;
	}

	return true;

}

bool NodeManager::addClient(long address, short id) {

	nodes[address] = new NodeItem(IDLE, 0, address, id);

	totalBackup = nodes.size() == 1 ? 0 : (uint32_t) ceil(nodes.size() * backupRate);

	readyBackup = 0;

	LOG_U(UI_UPDATE_DIST_BACKUP, totalBackup, readyBackup);
	LOG_T("Node at address : %s added to the list", Address::getString(address).c_str());

	return true;
}

NodeItem *NodeManager::getIdleClient(long collectorAddress) {

//TODO coklu collector de burada thread korumasi olmasi lazim

	NodeItem *leastUsedClient = nullptr;

	int idleCount = 0;

	for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
		NodeItem *clientMap = i->second;
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

	if (leastUsedClient != nullptr && idleCount - totalBackup + readyBackup > 0) {
		leastUsedClient->state = PREBUSY;

		initClientTimer(leastUsedClient, collectorAddress);

		LOG_T("Node at address : %s returned to collector",
			  Address::getString(leastUsedClient->address).c_str());

		return leastUsedClient;

	} else {
		LOG_W("No available client right now.");
	}

	return nullptr;
}

bool NodeManager::resetDiffTimes() {

	for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
		i->second->stopWatch.reset();
	}
	return true;
}

void NodeManager::clear() {

	nodes.clear();
	readyBackup = 0;
	totalBackup = 0;
}
