//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_NODEOBJECT_H
#define DICON_NODEOBJECT_H

#include "NodeState.h"
#include "ComponentUnit.h"
#include "ProcessItem.h"
#include "ComponentObject.h"

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
