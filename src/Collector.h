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

	Connector *mDistributorConnector;
	Connector *mClientConnector;

	std::map<uint64_t , Rule*> mRules;
	std::string mRootPath;
	CallBack *mCallback;
	uint64_t mDistributorAddress;

	static bool receiveCB(void *, uint64_t, Message *);

	bool processDistributorMsg(uint64_t address, Message *msg);
	bool processClientMsg(uint64_t address, Message *msg);

	bool send2DistributorMsg(uint64_t, uint8_t);
	bool send2ClientMsg(uint64_t, uint8_t);

public:
	Collector(uint32_t, uint32_t, const std::string &rootPath);

	virtual ~Collector();
	uint64_t getAddress(HOST);
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

