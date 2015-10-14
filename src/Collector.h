//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef COLLECTOR_H
#define	COLLECTOR_H

#include "Component.h"
#include "Rule.h"
#include "Connector.h"
#include "Message.h"

class Collector : public Component {

    int distributorIndex;
    int nodeIndex;

	std::map<long , Rule*> rules;

	long distributorAddress;

	virtual bool onReceive(long, Message *);

	bool processDistributorMsg(long address, Message *msg);
	bool processClientMsg(long address, Message *msg);

	bool send2DistributorMsg(long, int);
	bool send2ClientMsg(long, int);

public:
	Collector(int, int, const char *rootPath);

	virtual ~Collector();
	long getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	bool reset();
	bool processRule();
	bool processRule(const std::string &);
	bool syncTime();

	void display();
};

#endif	/* COLLECTOR_H */

