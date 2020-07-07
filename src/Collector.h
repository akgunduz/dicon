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
#include "DistributorObject.h"
#include "CollectorObject.h"

class Collector : public Component {

	Job *job;
	DistributorObject distributor;

    explicit Collector(const char *rootPath);

	ComponentObject& getDistributor();
	void setDistributor(const DistributorObject&);

	bool processDistributorWakeupMsg(const ComponentObject&, Message *);
	bool processDistributorIDMsg(const ComponentObject&, Message *);
	bool processDistributorNodeMsg(const ComponentObject&, Message *);
	bool processNodeInfoMsg(const ComponentObject&, Message *);
	bool processNodeBinaryMsg(const ComponentObject&, Message *);

	bool send2DistributorAliveMsg(const ComponentObject&);
	bool send2DistributorIDMsg(const ComponentObject&);
	bool send2DistributorNodeMsg(const ComponentObject&, long);
    bool send2DistributorReadyMsg(const ComponentObject&);
    bool send2NodeJobMsg(const ComponentObject&, const char*, int, const char*, const TypeFileInfoList&);
	bool send2NodeBinaryMsg(const ComponentObject&, const char*, int, const char*, const TypeFileInfoList&);
	bool send2NodeReadyMsg(const ComponentObject&);

public:

    ~Collector();
    static Collector* newInstance(const char*);

    Job* getJob();
    bool processJob();
    bool loadJob(const char*);
};

#endif	/* COLLECTOR_H */

