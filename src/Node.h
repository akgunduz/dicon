//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"

class Node : public Component {
private:

	Job* job = nullptr;

	long distributorAddress;
	bool setDistributorAddress(long);

    virtual bool processDistributorMsg(long, Message *);
    virtual bool processCollectorMsg(long, Message *);
    virtual bool processNodeMsg(long, Message *);

    bool send2DistributorMsg(long, uint8_t);
    bool send2CollectorMsg(long, uint8_t);

    void parseCommand(char *cmd, char **argv);
    bool processRule(Rule*);
    bool processMD5();
    bool processParallel(Rule* rule);
    bool processSequential(Rule* rule);

public:
	Node(const char *rootPath);

	virtual ~Node();
};


#endif //NODE_H
