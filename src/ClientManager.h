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

#define CHECK_INTERVAL 600
#define CLIENT_TIMEOUT 300

typedef bool (*fTimeoutCB)(Connector *, Address* , Address*);
typedef bool (*fWakeupCB)(Connector *);

struct ClientMap {

	volatile bool is_timer_active = false;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_t thread;

	STATES state = IDLE;
	int usage;
	long address;
	DiffTime diffTime;
	ClientMap(STATES s, int u, long a) : state(s), usage(u), address(a){
		diffTime.reset();
	}
};

class ClientManager {
private:

	Connector *clientConnector;

	std::map<long, ClientMap*> clients;

	uint32_t readyBackup = 0;
	uint32_t totalBackup = 0;
	double backupRate = 0;

	fTimeoutCB timeoutCB;
	fWakeupCB wakeupCB;

	pthread_mutex_t mutexClientChecker;
	pthread_cond_t condClientChecker;
	pthread_t threadClientChecker;

	bool initClientTimer(ClientMap *, Address*);
	bool stopClientTimer(ClientMap *);
	static void *runClientTimer(void *);
	static void *runClientChecker(void *);
	bool stopClientChecker();

public:

	ClientManager(Connector *, fTimeoutCB, fWakeupCB, double);

	virtual ~ClientManager();

	bool initClientChecker();

	bool resetDiffTimes();
	bool setClientIdle(Address*, double);
	bool setClientBusy(Address*);
	bool setClientRemove(Address*);
	bool setClientValidate(Address*);
	bool addClient(Address*);

	Address* getIdleClient(Address*);

	void clear();

};

struct ClientManagerArgument {
	Connector *clientConnector;
	ClientMap *clientMap;
	Address* collectorAddress;
	fTimeoutCB timeoutCB;
	ClientManagerArgument(Connector *cc, fTimeoutCB cb, ClientMap *c, Address* a) :
			clientConnector(cc), timeoutCB(cb), clientMap(c), collectorAddress(a) {}
};


#endif	/* DISTRIBUTOR_H */
