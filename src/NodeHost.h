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

    ProcessItem processItem{HostUnit(COMP_NODE)};

public:

    NodeHost(NODESTATES, int, long);
    explicit NodeHost(long);
    explicit NodeHost(const char*);
    NodeHost();
    NodeHost(const NodeHost &);

    ~NodeHost() override;

    ProcessItem& getProcess();

};

#endif //DICON_NODEHOST_H
