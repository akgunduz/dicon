//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef COLLECTOR_H
#define	COLLECTOR_H

#include "Rule.h"
#include "Connector.h"
#include "Message.h"

class Collector {

	Connector *distributorConnector;
	Connector *clientConnector;

	std::map<long , Rule*> rules;
	std::string rootPath;
	InterfaceCallback *callback;
	Address* distributorAddress;

	static bool receiveCB(void *, Address*, Message *);

	bool processDistributorMsg(Address* address, Message *msg);
	bool processClientMsg(Address* address, Message *msg);

	bool send2DistributorMsg(Address*, int);
	bool send2ClientMsg(Address*, int);

public:
	Collector(uint32_t, uint32_t, const std::string &rootPath);

	virtual ~Collector();
	Address* getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	bool reset();
	bool processRule();
	bool processRule(const std::string &);
	bool syncTime();

	void display();

	std::string getRootPath();
	void setRootPath(const std::string&);
};

#endif	/* COLLECTOR_H */

