//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_DISTRIBUTOR_H
#define	DICON_DISTRIBUTOR_H

#include "Component.h"
#include "Node.h"
#include "Connector.h"
#include "Util.h"
#include "NodeManager.h"
#include "CollectorManager.h"
#include "DistributorObject.h"

#define WAKEUP_INTERVAL 5

typedef std::map<long, TypeComponentList> TypeComponentReplaceIDList;

class Distributor : public Component {

    static Distributor *instance;

    volatile bool runCollThread = true;

    volatile bool runPollThread = true;

	NodeManager *nodeManager;

	CollectorManager *collectorManager;

    std::thread collThread;
    std::thread pollThread;

    explicit Distributor(const char *, int, int, bool);

	void pollProcess();
    void collProcess();

    bool processCollectorAliveMsg(ComponentUnit&, Message *);
    bool processCollectorIDMsg(ComponentUnit&, Message *);
    bool processCollectorNodeMsg(ComponentUnit&, Message *);
    bool processCollectorReadyMsg(ComponentUnit&, Message *);

    bool processNodeAliveMsg(ComponentUnit&, Message *);
    bool processNodeIDMsg(ComponentUnit&, Message *);
    bool processNodeBusyMsg(ComponentUnit&, Message *);
    bool processNodeReadyMsg(ComponentUnit&, Message *);

	bool send2CollectorWakeupMsg(ComponentUnit&);
	bool send2CollectorIDMsg(ComponentUnit&, long);
	bool send2CollectorNodeMsg(ComponentUnit&, TypeComponentList&);

    bool send2NodeWakeupMsg(ComponentUnit&);
    bool send2NodeIDMsg(ComponentUnit&, long);
	bool send2NodeProcessMsg(ComponentUnit&);

public:


	~Distributor() override;
    static Distributor* newInstance(const char*, int, int, bool);

    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll(bool);

    bool clear();
    CollectorManager* getCollectors() const;
    NodeManager* getNodes() const;
};

#endif //DICON_DISTRIBUTOR_H
