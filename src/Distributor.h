//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_DISTRIBUTOR_H
#define	DICON_DISTRIBUTOR_H

#include "Component.h"

class CollectorManager;
class NodeManager;

class Distributor : public Component {

    volatile bool runCollThread = true;

    volatile bool runPollThread = true;

    volatile bool autoWake = true;

	NodeManager *nodeManager;

	CollectorManager *collectorManager;

    std::thread collThread;
    std::thread pollThread;

	void pollProcess();
    void collProcess();

    bool processCollectorAliveMsg(const TypeComponentUnit&, TypeMessage);
    bool processCollectorIDMsg(const TypeComponentUnit&, TypeMessage);
    bool processCollectorNodeMsg(const TypeComponentUnit&, TypeMessage);
    bool processCollectorReadyMsg(const TypeComponentUnit&, TypeMessage);

    bool processNodeAliveMsg(const TypeComponentUnit&, TypeMessage);
    bool processNodeIDMsg(const TypeComponentUnit&, TypeMessage);
    bool processNodeBusyMsg(const TypeComponentUnit&, TypeMessage);
    bool processNodeReadyMsg(const TypeComponentUnit&, TypeMessage);

	bool send2CollectorWakeupMsg(const TypeComponentUnit&);
	bool send2CollectorIDMsg(const TypeComponentUnit&, TypeID);
	bool send2CollectorNodeMsg(const TypeComponentUnit&, TypeComponentUnitList&);
	bool send2CollectorReplaceMsg(const TypeComponentUnit&, TypeComponentUnitList&);

    bool send2NodeWakeupMsg(const TypeComponentUnit&);
    bool send2NodeIDMsg(const TypeComponentUnit&, TypeID);
	bool send2NodeProcessMsg(const TypeComponentUnit&);

public:

    Distributor(int, int, bool);
	~Distributor() override;

    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll(bool);

    bool clear();
    CollectorManager* getCollectors() const;
    NodeManager* getNodes() const;

    volatile bool getAutoWake() const;
};

typedef std::shared_ptr<Distributor> TypeDistributor;

#endif //DICON_DISTRIBUTOR_H
