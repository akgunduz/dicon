//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef CLIENTMANAGER_H
#define	CLIENTMANAGER_H

#include "Client.h"
#include "Connector.h"
#include "Tools.h"

#define CHECK_INTERVAL 600
#define CLIENT_TIMEOUT 300

typedef bool (*fTimeoutCB)(Connector *, long , long);
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

	bool initClientTimer(ClientMap *, long);
	bool stopClientTimer(ClientMap *);
	static void *runClientTimer(void *);
	static void *runClientChecker(void *);
	bool stopClientChecker();

public:

	ClientManager(Connector *, fTimeoutCB, fWakeupCB, double);

	virtual ~ClientManager();

	bool initClientChecker();

	bool resetDiffTimes();
	bool setClientIdle(long, double);
	bool setClientBusy(long);
	bool setClientRemove(long);
	bool setClientValidate(long);
	bool addClient(long);

	long getIdleClient(long);

	void clear();

};

struct ClientManagerArgument {
	Connector *clientConnector;
	ClientMap *clientMap;
	long collectorAddress;
	fTimeoutCB timeoutCB;
	ClientManagerArgument(Connector *cc, fTimeoutCB cb, ClientMap *c, long a) :
			clientConnector(cc), timeoutCB(cb), clientMap(c), collectorAddress(a) {}
};


#endif	/* DISTRIBUTOR_H */
