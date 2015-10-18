//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Client_H_
#define __Client_H_

#include "Component.h"

class Node : public Component {
private:

	Rule* rule;

	long distributorAddress;

    virtual bool processDistributorMsg(long, Message *);
    virtual bool processCollectorMsg(long, Message *);
    virtual bool processNodeMsg(long, Message *);

    bool send2DistributorMsg(long, uint8_t);
    bool send2CollectorMsg(long, uint8_t);

    bool processRule();
    void processExecutor(std::string);
    bool processMD5();

public:
	Node(int, int, const char *rootPath);

	virtual ~Node();
};


#endif //__Client_H_
