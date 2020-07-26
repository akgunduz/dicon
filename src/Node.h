//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"
#include "NodeObject.h"
#include "DistributorObject.h"

class Node : public Component {
private:

	DistributorObject distributor;

    std::vector<ProcessItem> processList;

	explicit Node(const char *, int);

	ComponentObject getDistributor();
	void setDistributor(const DistributorObject&);

    bool processDistributorWakeupMsg(const ComponentObject&, Message *);
    bool processDistributorIDMsg(const ComponentObject&, Message *);
    bool processDistributorProcessMsg(const ComponentObject&, Message *);

    bool processCollectorProcessMsg(const ComponentObject &owner, Message *msg);
    bool processCollectorBinaryMsg(const ComponentObject&, Message *);
    bool processCollectorReadyMsg(const ComponentObject&, Message *);

    bool send2DistributorReadyMsg(const ComponentObject&);
    bool send2DistributorAliveMsg(const ComponentObject&);
    bool send2DistributorIDMsg(const ComponentObject&);
    bool send2DistributorBusyMsg(const ComponentObject&, long);

    bool send2CollectorInfoMsg(const ComponentObject&, long, TypeProcessFileList&);
	bool send2CollectorBinaryMsg(const ComponentObject&, long, TypeProcessFileList&);

    bool processJob(const ComponentObject&, Message *);
    void parseCommand(char *cmd, char **argv);
    bool processCommand(long, long, const char *);

public:

	~Node() override;
    static Node* newInstance(const char*, int);

    std::vector<ProcessItem>& getProcessList();
};


#endif //NODE_H
