//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"
#include "NodeHost.h"
#include "DistributorObject.h"

class Node : public Component {
private:

    ComponentUnit distributor;

    std::vector<ProcessItem> processList;

	explicit Node(const char *, int);

	void setDistributor(const ComponentUnit&);

    bool processDistributorWakeupMsg(ComponentUnit&, Message *);
    bool processDistributorIDMsg(ComponentUnit&, Message *);
    bool processDistributorProcessMsg(ComponentUnit&, Message *);

    bool processCollectorProcessMsg(ComponentUnit &owner, Message *msg);
    bool processCollectorBinaryMsg(ComponentUnit&, Message *);
    bool processCollectorReadyMsg(ComponentUnit&, Message *);

    bool send2DistributorReadyMsg(ComponentUnit&);
    bool send2DistributorAliveMsg(ComponentUnit&);
    bool send2DistributorIDMsg(ComponentUnit&);
    bool send2DistributorBusyMsg(ComponentUnit&, long);

    bool send2CollectorInfoMsg(ComponentUnit&, long, TypeProcessFileList&);
	bool send2CollectorBinaryMsg(ComponentUnit&, long, TypeProcessFileList&);

    bool processJob(const ComponentUnit&, Message *);
    void parseCommand(char *cmd, char **argv);
    bool processCommand(long, long, const char *);

public:

	~Node() override;
    static Node* newInstance(const char*, int);

    std::vector<ProcessItem>& getProcessList();
};


#endif //NODE_H
