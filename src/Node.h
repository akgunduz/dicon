//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_NODE_H
#define DICON_NODE_H

#include "Component.h"
#include "NodeHost.h"
#include "DistributorObject.h"

class Node : public Component {

    ComponentUnit distributor;

    TypeProcessItem processItem;

    TypeProcessList processList;

	void setDistributor(const ComponentUnit&);

    bool processDistributorWakeupMsg(ComponentUnit&, TypeMessage);
    bool processDistributorIDMsg(ComponentUnit&, TypeMessage);
    bool processDistributorProcessMsg(ComponentUnit&, TypeMessage);

    bool processCollectorProcessMsg(ComponentUnit&, TypeMessage);
    bool processCollectorBinaryMsg(ComponentUnit&, TypeMessage);
    bool processCollectorReadyMsg(ComponentUnit&, TypeMessage);

    bool send2DistributorReadyMsg(ComponentUnit&);
    bool send2DistributorAliveMsg(ComponentUnit&);
    bool send2DistributorIDMsg(ComponentUnit&);
    bool send2DistributorBusyMsg(ComponentUnit&, long);

    bool send2CollectorInfoMsg(ComponentUnit&, long, TypeProcessFileList&);
	bool send2CollectorBinaryMsg(ComponentUnit&, long, TypeProcessFileList&);

    bool processJob(const ComponentUnit&, TypeMessage);
    void parseCommand(char *cmd, char **argv);
    bool processCommand(long, long, const std::string&);

public:

    explicit Node(int);
	~Node() override;

    TypeProcessList& getProcessList();
};

typedef std::unique_ptr<Node> TypeNode;

#endif //DICON_NODE_H
