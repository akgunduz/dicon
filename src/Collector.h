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
    TypeComponentUnit distributor;

	void setDistributor(const TypeComponentUnit&);

	bool processDistributorWakeupMsg(const TypeComponentUnit&, TypeMessage);
	bool processDistributorIDMsg(const TypeComponentUnit&, TypeMessage);
	bool processDistributorNodeMsg(const TypeComponentUnit&, TypeMessage);
	bool processDistributorReplaceMsg(const TypeComponentUnit&, TypeMessage);
	bool processNodeFileInfoMsg(const TypeComponentUnit&, TypeMessage);
	bool processNodeFileBinaryMsg(const TypeComponentUnit&, TypeMessage);

	bool send2DistributorAliveMsg(const TypeComponentUnit&);
	bool send2DistributorIDMsg(const TypeComponentUnit&);
	bool send2DistributorNodeMsg(const TypeComponentUnit&, long);
    bool send2DistributorReadyMsg(const TypeComponentUnit&);
    bool send2NodeProcessMsg(const TypeComponentUnit&, const TypeProcessItem&);
	bool send2NodeBinaryMsg(const TypeComponentUnit&, long, const TypeProcessFileList&);
	bool send2NodeReadyMsg(const TypeComponentUnit&);

public:

    Collector(int, int);
    ~Collector() override;

    TypeJobItem& getJob();
    bool processJob();
    TypeJobItem& loadJob(const std::filesystem::path&);
};

typedef std::unique_ptr<Collector> TypeCollector;

#endif //DICON_COLLECTOR_H

