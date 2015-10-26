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
#include "Job.h"

class Collector : public Component {

	std::map<long , Job*> jobs;

	long distributorAddress;

	virtual bool processDistributorMsg(long, Message *);
	virtual bool processCollectorMsg(long, Message *);
	virtual bool processNodeMsg(long, Message *);

	bool send2DistributorMsg(long, int);
	bool send2ClientMsg(long, int);

public:

	Collector(int, int, const char *rootPath);
    virtual ~Collector();

    bool reset();
    bool processRule();
    bool processRule(const std::string &);
    bool syncTime();

    void display();
};

#endif	/* COLLECTOR_H */

