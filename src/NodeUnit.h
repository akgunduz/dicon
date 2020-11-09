//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODEUNIT_H
#define DICON_NODEUNIT_H

#include "ComponentUnit.h"
#include "NodeObject.h"

class NodeUnit : public ComponentUnit, public NodeObject {

public:
    NodeUnit(NODESTATES, int, ARCH, long, TypeAddress);
    explicit NodeUnit(ARCH, long, TypeAddress);
    NodeUnit(const NodeUnit &);

    ~NodeUnit() override;
};

typedef std::shared_ptr<NodeUnit> TypeNodeUnit;
typedef std::vector<TypeNodeUnit> TypeNodeList;

#endif //DICON_NODEUNIT_H
