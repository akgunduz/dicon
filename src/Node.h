//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"

class Node : public Component {
private:

	long distributorAddress;

	Node(const char *rootPath);

	long getDistributorAddress();
	void setDistributorAddress(long);

    bool processDistributorWakeupMsg(ComponentObject, long, Message *);
    bool processDistributorIDMsg(ComponentObject, long, Message *);

    bool processCollectorJobMsg(ComponentObject, long, Message *);
    bool processCollectorBinaryMsg(ComponentObject, long, Message *);
    bool processCollectorReadyMsg(ComponentObject, long, Message *);

    bool send2DistributorReadyMsg(long);
    bool send2DistributorAliveMsg(long);
    bool send2DistributorIDMsg(long);
    bool send2DistributorBusyMsg(long);

    bool send2CollectorInfoMsg(long, const char*, long, const char*, TypeFileInfoList*);
	bool send2CollectorBinaryMsg(long, const char*, long, const char*, TypeFileInfoList*);

    void parseCommand(char *cmd, char **argv);
    bool processCommand(const char*);

public:


	~Node();
    static Node* newInstance(const char*);
};


#endif //NODE_H
