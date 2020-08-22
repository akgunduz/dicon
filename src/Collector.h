//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTOR_H
#define	DICON_COLLECTOR_H

#include "Component.h"
#include "InterfaceFactory.h"
#include "Message.h"
#include "JobItem.h"
#include "DistributorObject.h"
#include "CollectorObject.h"

class Collector : public Component {

    TypeJobItem job;
    ComponentUnit distributor{};

	void setDistributor(const ComponentUnit&);

	bool processDistributorWakeupMsg(ComponentUnit&, TypeMessage);
	bool processDistributorIDMsg(ComponentUnit&, TypeMessage);
	bool processDistributorNodeMsg(ComponentUnit&, TypeMessage);
	bool processDistributorReplaceMsg(ComponentUnit&, TypeMessage);
	bool processNodeFileInfoMsg(ComponentUnit&, TypeMessage);
	bool processNodeFileBinaryMsg(ComponentUnit&, TypeMessage);

	bool send2DistributorAliveMsg(ComponentUnit&);
	bool send2DistributorIDMsg(ComponentUnit&);
	bool send2DistributorNodeMsg(ComponentUnit&, long);
    bool send2DistributorReadyMsg(ComponentUnit&);
    bool send2NodeProcessMsg(ComponentUnit&, const TypeProcessItem&);
	bool send2NodeBinaryMsg(ComponentUnit&, long, TypeProcessFileList&);
	bool send2NodeReadyMsg(ComponentUnit&);

public:

    Collector(int, int);
    ~Collector() override;

    TypeJobItem& getJob();
    bool processJob();
    TypeJobItem& loadJob(const char*);
};

typedef std::unique_ptr<Collector> TypeCollector;

#endif //DICON_COLLECTOR_H

