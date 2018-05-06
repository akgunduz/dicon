//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"

class Node : public Component {
private:

	ComponentObject distributor;
	long distributorAddress;

	Node(const char *rootPath);

	ComponentObject getDistributor();
	void setDistributor(ComponentObject);

    bool processDistributorWakeupMsg(ComponentObject, Message *);
    bool processDistributorIDMsg(ComponentObject, Message *);
    bool processDistributorProcessMsg(ComponentObject, Message *);

    bool processCollectorJobMsg(ComponentObject, Message *);
    bool processCollectorBinaryMsg(ComponentObject, Message *);
    bool processCollectorReadyMsg(ComponentObject, Message *);

    bool send2DistributorReadyMsg(ComponentObject, const char*, long, long);
    bool send2DistributorAliveMsg(ComponentObject);
    bool send2DistributorIDMsg(ComponentObject);
    bool send2DistributorBusyMsg(ComponentObject, const char*, long, const char *,
								 TypeFileInfoList *, long);

    bool send2CollectorInfoMsg(ComponentObject, const char*, long, const char*, TypeFileInfoList*);
	bool send2CollectorBinaryMsg(ComponentObject, const char*, long, const char*, TypeFileInfoList*);

    void parseCommand(char *cmd, char **argv);
    bool processCommand(int, const char*, const char *);

public:


	~Node();
    static Node* newInstance(const char*);
};


#endif //NODE_H
