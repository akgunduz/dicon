//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef COLLECTOR_H
#define	COLLECTOR_H

#include "Component.h"
#include "Connector.h"
#include "Message.h"
#include "JobItem.h"
#include "DistributorObject.h"
#include "CollectorObject.h"

class Collector : public Component {

	JobItem *job;
	DistributorObject distributor;

    explicit Collector(const char *, int, int);

	void setDistributor(const DistributorObject&);

	bool processDistributorWakeupMsg(const ComponentObject&, Message *);
	bool processDistributorIDMsg(const ComponentObject&, Message *);
	bool processDistributorNodeMsg(const ComponentObject&, Message *);
	bool processNodeFileInfoMsg(const ComponentObject &owner, Message *msg);
	bool processNodeFileBinaryMsg(const ComponentObject &owner, Message *msg);

	bool send2DistributorAliveMsg(const ComponentObject&);
	bool send2DistributorIDMsg(const ComponentObject&);
	bool send2DistributorNodeMsg(const ComponentObject&, long);
    bool send2DistributorReadyMsg(const ComponentObject&);
    bool send2NodeProcessMsg(const ComponentObject&, ProcessItem *);
	bool send2NodeBinaryMsg(const ComponentObject&, long, TypeProcessFileList&);
	bool send2NodeReadyMsg(const ComponentObject&);

public:

    ~Collector() override;
    static Collector* newInstance(const char*, int, int);

    JobItem* getJob();
    bool processJob();
    JobItem* loadJob(const char*);
};

#endif	/* COLLECTOR_H */

