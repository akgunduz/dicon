//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef COLLECTOR_H
#define	COLLECTOR_H

#include "Component.h"
#include "Connector.h"
#include "Message.h"
#include "Job.h"
#include "Jobs.h"

class Collector : public Component {

	Jobs jobs;

	long distributorAddress;
	bool setDistributorAddress(long);

	bool processDistributorWakeupMsg(long, Message *);
	bool processDistributorNodeMsg(long, Message *);
	bool processNodeInfoMsg(long, Message *);

	bool send2DistributorMsg(long, MSG_TYPE, ...);
	bool send2NodeMsg(long, MSG_TYPE, ...);

public:

	Collector(const char *rootPath);
    virtual ~Collector();

    bool processJob();
	Jobs* getJobs();

};

#endif	/* COLLECTOR_H */

