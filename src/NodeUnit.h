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
    NodeUnit(NODESTATES, int, ARCH, long, Address);
    explicit NodeUnit(ARCH, long, Address);
    NodeUnit(const NodeUnit &);

    ~NodeUnit() override;
};

typedef std::shared_ptr<NodeUnit> TypeNodeUnit;
typedef std::vector<TypeNodeUnit> TypeNodeList;

#endif //DICON_NODEUNIT_H
