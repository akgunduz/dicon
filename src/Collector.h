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

	std::vector<Job*> *jobs;

	long distributorAddress;

	virtual bool processDistributorMsg(long, Message *);
	virtual bool processCollectorMsg(long, Message *);
	virtual bool processNodeMsg(long, Message *);

	bool send2DistributorMsg(long, int);
	bool send2NodeMsg(long, int, FileList *);

    Job* getJob(long id);
    Job* getAttachedJob(long address);
    Job* getUnservedJob();

public:

	Collector(const char *rootPath);
    virtual ~Collector();

    bool reset();
    bool processRule();
    bool syncTime();

};

#endif	/* COLLECTOR_H */

