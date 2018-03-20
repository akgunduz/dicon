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
#include "Jobs.h"

class Collector : public Component {

	Jobs jobs;

	long distributorAddress;

	virtual bool processDistributorMsg(long, Message *);
	virtual bool processCollectorMsg(long, Message *);
	virtual bool processNodeMsg(long, Message *);

	bool send2DistributorMsg(long, int);
	bool send2NodeMsg(long, int, FileList *);

public:

	Collector(const char *rootPath);
    virtual ~Collector();

    bool processRule();
    bool syncTime();
	Jobs* getJobs();

};

#endif	/* COLLECTOR_H */

