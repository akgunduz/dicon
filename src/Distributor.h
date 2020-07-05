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

    bool processCollectorAliveMsg(ComponentObject, Message *);
    bool processCollectorIDMsg(ComponentObject, Message *);
    bool processCollectorNodeMsg(ComponentObject, Message *);

    bool processNodeAliveMsg(ComponentObject, Message *);
    bool processNodeIDMsg(ComponentObject, Message *);
    bool processNodeBusyMsg(ComponentObject, Message *);
    bool processNodeReadyMsg(ComponentObject, Message *);

	bool send2CollectorWakeupMsg(ComponentObject);
	bool send2CollectorIDMsg(ComponentObject, int);
	bool send2CollectorNodeMsg(ComponentObject, std::vector<ComponentObject>&);

    bool send2NodeWakeupMsg(ComponentObject);
    bool send2NodeIDMsg(ComponentObject, int);
	bool send2NodeProcessMsg(ComponentObject,
								   const char*, long, const char *,
								   TypeFileInfoList *, int, long);

public:


	~Distributor() override;
    static Distributor* newInstance(const char* path);

    bool sendWakeupMessage(ComponentObject);
    bool sendWakeupMessagesAll(bool);

    bool clear();
    CollectorManager* getCollectors() const;
    NodeManager* getNodes() const;
};

#endif	/* DISTRIBUTOR_H */
