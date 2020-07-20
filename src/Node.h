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

    std::vector<ProcessInfo> processList;

	explicit Node(const char *rootPath);

	ComponentObject getDistributor();
	void setDistributor(const DistributorObject&);

    bool processDistributorWakeupMsg(const ComponentObject&, Message *);
    bool processDistributorIDMsg(const ComponentObject&, Message *);
    bool processDistributorProcessMsg(const ComponentObject&, Message *);

    bool processCollectorJobMsg(const ComponentObject&, Message *);
    bool processCollectorBinaryMsg(const ComponentObject&, Message *);
    bool processCollectorReadyMsg(const ComponentObject&, Message *);

    bool send2DistributorReadyMsg(const ComponentObject&);
    bool send2DistributorAliveMsg(const ComponentObject&);
    bool send2DistributorIDMsg(const ComponentObject&);
    bool send2DistributorBusyMsg(const ComponentObject&, int);

    bool send2CollectorInfoMsg(const ComponentObject&, const char*, int, const char*, TypeFileInfoList&);
	bool send2CollectorBinaryMsg(const ComponentObject&, const char*, int, const char*, TypeFileInfoList&);

    bool processJob(const ComponentObject&, Message *);
    void parseCommand(char *cmd, char **argv);
    bool processCommand(int, int, const char *);

public:

	~Node() override;
    static Node* newInstance(const char*);

    std::vector<ProcessInfo>& getProcessList();
};


#endif //NODE_H
