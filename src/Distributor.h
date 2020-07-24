//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DISTRIBUTOR_H
#define	DISTRIBUTOR_H

#include "Component.h"
#include "Node.h"
#include "Connector.h"
#include "Util.h"
#include "NodeManager.h"
#include "CollectorManager.h"
#include "DistributorObject.h"

class Distributor : public Component {
private:

    static Distributor *instance;

    volatile bool runCollThread = true;

	NodeManager *nodeManager;

	CollectorManager *collectorManager;

    std::thread collThread;

    explicit Distributor(const char *);

    static void collProcessCB(Distributor *);
    void collProcess();

    bool processCollectorAliveMsg(const ComponentObject&, Message *);
    bool processCollectorIDMsg(const ComponentObject&, Message *);
    bool processCollectorNodeMsg(const ComponentObject&, Message *);
    bool processCollectorReadyMsg(const ComponentObject&, Message *);

    bool processNodeAliveMsg(const ComponentObject&, Message *);
    bool processNodeIDMsg(const ComponentObject&, Message *);
    bool processNodeBusyMsg(const ComponentObject&, Message *);
    bool processNodeReadyMsg(const ComponentObject&, Message *);

	bool send2CollectorWakeupMsg(const ComponentObject&);
	bool send2CollectorIDMsg(const ComponentObject&, long);
	bool send2CollectorNodeMsg(const ComponentObject&, std::vector<ComponentObject>&);

    bool send2NodeWakeupMsg(const ComponentObject&);
    bool send2NodeIDMsg(const ComponentObject&, long);
	bool send2NodeProcessMsg(const ComponentObject&);

public:


	~Distributor() override;
    static Distributor* newInstance(const char* path);

    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll(bool);

    bool clear();
    CollectorManager* getCollectors() const;
    NodeManager* getNodes() const;
};

#endif	/* DISTRIBUTOR_H */
