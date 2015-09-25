//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef CLIENTMANAGER_H
#define	CLIENTMANAGER_H

#include <atomic>
#include "Client.h"
#include "Connector.h"
#include "Tools.h"

#define CHECK_INTERVAL 30
#define CLIENT_TIMEOUT 10

typedef bool (*fTimeoutCB)(Connector *, uint64_t, uint64_t);
typedef bool (*fWakeupCB)(Connector *);

struct ClientMap {
	volatile bool is_timer_active = false;

	pthread_mutex_t mMutex;
	pthread_cond_t mCond;
	pthread_t mThread;

	STATES state = IDLE;
	int usage;
	uint64_t address;
	DiffTime diffTime;
	ClientMap(STATES s, int u, uint64_t a) : state(s), usage(u), address(a){
		diffTime.reset();
	}
};

class ClientManager {
private:

	Connector *mClientConnector;

	std::map<uint64_t, ClientMap*> mClients;

	uint32_t mReadyBackup = 0;
	uint32_t mTotalBackup = 0;
	double mBackupRate = 0;

	fTimeoutCB mTimeoutCB;
	fWakeupCB mWakeupCB;

	pthread_mutex_t mMutexClientChecker;
	pthread_cond_t mCondClientChecker;
	pthread_t mThreadClientChecker;

	bool initClientTimer(ClientMap *, uint64_t);
	bool stopClientTimer(ClientMap *);
	static void *runClientTimer(void *);
	static void *runClientChecker(void *);
	bool stopClientChecker();

public:

	ClientManager(Connector *, fTimeoutCB, fWakeupCB, double);

	virtual ~ClientManager();

	bool initClientChecker();

	bool resetDiffTimes();
	bool setClientIdle(uint64_t, double);
	bool setClientBusy(uint64_t);
	bool setClientRemove(uint64_t);
	bool setClientValidate(uint64_t);
	bool addClient(uint64_t);

	uint64_t getIdleClient(uint64_t);

	void clear();

};

struct ClientManagerArgument {
	Connector *clientConnector;
	ClientMap *clientMap;
	uint64_t collectorAddress;
	fTimeoutCB timeoutCB;
	ClientManagerArgument(Connector *cc, fTimeoutCB cb, ClientMap *c, uint64_t a) :
			clientConnector(cc), timeoutCB(cb), clientMap(c), collectorAddress(a) {}
};


#endif	/* DISTRIBUTOR_H */
