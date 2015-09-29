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
	long distributorAddress;

	static bool receiveCB(void *, long, Message *);

	bool processDistributorMsg(long address, Message *msg);
	bool processClientMsg(long address, Message *msg);

	bool send2DistributorMsg(long, int);
	bool send2ClientMsg(long, int);

public:
	Collector(uint32_t, uint32_t, const std::string &rootPath);

	virtual ~Collector();
	long getAddress(HOST);
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

