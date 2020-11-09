//
// Created by Haluk AKGUNDUZ on 18.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODEHOST_H
#define DICON_NODEHOST_H

#include "HostUnit.h"
#include "NodeObject.h"

class NodeHost : public HostUnit, public NodeObject {

public:

    explicit NodeHost(long = 0, NODESTATES = NODESTATE_IDLE, int = 0);

    NodeHost(const NodeHost &);

    ~NodeHost() override;
};

typedef std::shared_ptr<NodeHost> TypeNodeHost;

#endif //DICON_NODEHOST_H
