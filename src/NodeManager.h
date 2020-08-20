//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODEMANAGER_H
#define	DICON_NODEMANAGER_H

#include "NodeUnit.h"
#include "ComponentManager.h"

class NodeManager : public ComponentManager {

    std::mutex nodeMutex;

public:

	explicit NodeManager(TypeHostUnit&, bool);
	~NodeManager() override;

    NODESTATES getState(long);
	void setState(long, NODESTATES);

    Address& getAddress(long);
    TypeNodeUnit getIdle(const TypeComponentUnit&);
    size_t getIdleCount();
    TypeNodeUnit getBusyDead();
    size_t getBusyDeadCount();

    TypeComponentUnit getAssigned(long);
	void setAssigned(long, TypeComponentUnit&);
	void setAssigned(long, ARCH, long, Address&);
	void releaseAssigned(long);

    TypeComponentUnit createUnit(ARCH, long, Address&) final;
};

#endif //DICON_NODEMANAGER_H