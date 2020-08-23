//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_NODEHOST_H
#define DICON_NODEHOST_H

#include "NodeState.h"
#include "HostUnit.h"
#include "ProcessItem.h"
#include "NodeObject.h"

class NodeHost : public HostUnit, public NodeObject {

public:

    explicit NodeHost(long = 0, NODESTATES = NODESTATE_IDLE, int = 0);

    NodeHost(const NodeHost &);

    ~NodeHost() override;
};

typedef std::shared_ptr<NodeHost> TypeNodeHost;

#endif //DICON_NODEHOST_H
