//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODEOBJECT_H
#define DICON_NODEOBJECT_H

#include "ComponentObject.h"
#include "NodeState.h"

class NodeObject : public ComponentObject {

    NODESTATES state{NODESTATE_IDLE};

    int usage{};

public:

    NodeObject() = default;
    NodeObject(NODESTATES, int);

    int getUsage() const;
    int iterateUsage(bool);

    NODESTATES getState();
    void setState(NODESTATES);
};

#endif //DICON_NODEOBJECT_H
