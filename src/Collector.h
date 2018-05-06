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
	ComponentObject distributor;
	long distributorAddress;

    Collector(const char *rootPath);

	ComponentObject getDistributor();
	void setDistributor(ComponentObject);

	bool processDistributorWakeupMsg(ComponentObject, Message *);
	bool processDistributorIDMsg(ComponentObject, Message *);
	bool processDistributorNodeMsg(ComponentObject, Message *);
	bool processNodeInfoMsg(ComponentObject, Message *);
	bool processNodeBinaryMsg(ComponentObject, Message *);

	bool send2DistributorAliveMsg(ComponentObject);
	bool send2DistributorNodeMsg(ComponentObject, const char*, TypeMD5List*);
	bool send2NodeJobMsg(ComponentObject, const char*, long, const char*, TypeFileInfoList*);
	bool send2NodeBinaryMsg(ComponentObject, const char*, long, const char*, TypeFileInfoList*);
	bool send2NodeReadyMsg(ComponentObject, const char*);

public:


    ~Collector();
    static Collector* newInstance(const char*);

    bool processJob(int);
    bool loadJob(const char*);
    bool processJobs();
	Jobs* getJobs();

};

#endif	/* COLLECTOR_H */

