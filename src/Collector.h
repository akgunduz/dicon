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
#include "DistributorObject.h"
#include "CollectorObject.h"

class Collector : public Component {

	Jobs jobs;
	DistributorObject distributor;

    explicit Collector(const char *rootPath);

	ComponentObject getDistributor();
	void setDistributor(const DistributorObject&);

	bool processDistributorWakeupMsg(ComponentObject, Message *);
	bool processDistributorIDMsg(ComponentObject, Message *);
	bool processDistributorNodeMsg(ComponentObject, Message *);
	bool processNodeInfoMsg(ComponentObject, Message *);
	bool processNodeBinaryMsg(ComponentObject, Message *);

	bool send2DistributorAliveMsg(ComponentObject);
	bool send2DistributorNodeMsg(ComponentObject, const char*, long, TypeMD5List*);
	bool send2NodeJobMsg(ComponentObject, const char*, long, const char*, TypeFileInfoList*);
	bool send2NodeBinaryMsg(ComponentObject, const char*, long, const char*, TypeFileInfoList*);
	bool send2NodeReadyMsg(ComponentObject, const char*, long);

public:

    ~Collector();
    static Collector* newInstance(const char*);

    bool processJob(int);
    bool loadJob(const char*);
    bool processJobs();
	Jobs* getJobs();
};

#endif	/* COLLECTOR_H */

