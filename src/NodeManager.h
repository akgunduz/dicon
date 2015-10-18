//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef CLIENTMANAGER_H
#define	CLIENTMANAGER_H

#include "Node.h"
#include "Connector.h"
#include "Util.h"
#include "StopWatch.h"

#define CHECK_INTERVAL 600
#define CLIENT_TIMEOUT 300

typedef bool (*fTimeoutCB)(Connector *, long , long);
typedef bool (*fWakeupCB)(Connector *);

struct NodeItem {

	volatile bool is_timer_active = false;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_t thread;

	NodeStates state = IDLE;
	int usage;
	long address;
	long lastServedCollector;
	short id;
	StopWatch stopWatch;
	NodeItem(NodeStates s, int u, long a, short i) : state(s), usage(u), address(a), id(i) {
		stopWatch.reset();
	}
};

class NodeManager {
private:

	Connector *clientConnector;

	std::map<long, NodeItem *> nodes;

	uint32_t readyBackup = 0;
	uint32_t totalBackup = 0;
	double backupRate = 0;

	fTimeoutCB timeoutCB;
	fWakeupCB wakeupCB;

	pthread_mutex_t mutexClientChecker;
	pthread_cond_t condClientChecker;
	pthread_t threadClientChecker;

	bool initClientTimer(NodeItem *, long);
	bool stopClientTimer(NodeItem *);
	static void *runClientTimer(void *);
	static void *runClientChecker(void *);
	bool stopClientChecker();

public:

	NodeManager(Connector *, fTimeoutCB, fWakeupCB, double);

	virtual ~NodeManager();

	bool initClientChecker();

	bool resetDiffTimes();
	bool setClientIdle(long, short, double);
	bool setClientBusy(long);
	bool setClientRemove(long);
	bool setClientValidate(long, short);
	bool addClient(long, short);

	NodeItem * getIdleClient(long);

	void clear();

};

struct ClientManagerArgument {
	Connector *clientConnector;
	NodeItem *clientMap;
	long collectorAddress;
	fTimeoutCB timeoutCB;
	ClientManagerArgument(Connector *cc, fTimeoutCB cb, NodeItem *c, long a) :
			clientConnector(cc), timeoutCB(cb), clientMap(c), collectorAddress(a) {}
};


#endif	/* DISTRIBUTOR_H */
