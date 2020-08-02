//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_NODEUNIT_H
#define DICON_NODEUNIT_H

#include "Address.h"
#include "NodeState.h"
#include "ComponentUnit.h"
#include "NodeObject.h"

class NodeUnit : public ComponentUnit, public NodeObject {

public:
    NodeUnit(NODESTATES, int, long, Address);
    explicit NodeUnit(long, Address);
    NodeUnit();
    NodeUnit(const NodeUnit &);

    ~NodeUnit();
};

typedef std::vector<NodeUnit> TypeNodeList;


#endif //DICON_NODEUNIT_H
