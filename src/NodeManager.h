//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODEMANAGER_H
#define	DICON_NODEMANAGER_H

#include "NodeUnit.h"
#include "ComponentManager.h"

class NodeManager : public ComponentManager {

public:

	explicit NodeManager(TypeHostUnit&, bool);
    ~NodeManager() override = default;

    NODESTATES getState(TypeID);
	void setState(TypeID, NODESTATES);

    TypeAddress getAddress(TypeID);
    TypeNodeUnit getIdle(const TypeComponentUnit&);
    size_t getIdleCount();
    TypeNodeUnit getBusyDead();
    size_t getBusyDeadCount();

    TypeComponentUnit getAssigned(TypeID);
	void setAssigned(TypeID, TypeComponentUnit&);
	void setAssigned(TypeID, ARCH, TypeID, TypeAddress&);
	void releaseAssigned(TypeID);

    TypeComponentUnit createUnit(ARCH, TypeID, TypeAddress&) final;
};

#endif //DICON_NODEMANAGER_H