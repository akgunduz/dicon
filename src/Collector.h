//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTOR_H
#define	DICON_COLLECTOR_H

#include "Component.h"
#include "Connector.h"
#include "Message.h"
#include "JobItem.h"
#include "DistributorObject.h"
#include "CollectorObject.h"

class Collector : public Component {

	JobItem *job{};
    ComponentUnit distributor{};

    explicit Collector(const char *, int, int);

	void setDistributor(const ComponentUnit&);

	bool processDistributorWakeupMsg(ComponentUnit&, Message *);
	bool processDistributorIDMsg(ComponentUnit&, Message *);
	bool processDistributorNodeMsg(ComponentUnit&, Message *);
	bool processNodeFileInfoMsg(ComponentUnit &owner, Message *msg);
	bool processNodeFileBinaryMsg(ComponentUnit &owner, Message *msg);

	bool send2DistributorAliveMsg(ComponentUnit&);
	bool send2DistributorIDMsg(ComponentUnit&);
	bool send2DistributorNodeMsg(ComponentUnit&, long);
    bool send2DistributorReadyMsg(ComponentUnit&);
    bool send2NodeProcessMsg(ComponentUnit&, ProcessItem *);
	bool send2NodeBinaryMsg(ComponentUnit&, long, TypeProcessFileList&);
	bool send2NodeReadyMsg(ComponentUnit&);

public:

    ~Collector() override;
    static Collector* newInstance(const char*, int, int);

    JobItem* getJob();
    bool processJob();
    JobItem* loadJob(const char*);
};

#endif //DICON_COLLECTOR_H

