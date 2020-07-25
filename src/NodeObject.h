//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_NODEOBJECT_H
#define DICON_NODEOBJECT_H

#include "NodeState.h"
#include "ComponentObject.h"
#include "ProcessItem.h"

class NodeObject : public ComponentObject {

private:

    NODESTATES state{};

    int usage{};

    ProcessItem processItem{ComponentObject(COMP_NODE)};

public:

    NodeObject(NODESTATES, int, long, long);
    NodeObject(long, long);
    explicit NodeObject(const char*);
    NodeObject();
    NodeObject(const NodeObject &);
    explicit NodeObject(const ComponentObject &);

    ~NodeObject() override;

    int getUsage() const;
    int iterateUsage(bool);

    NODESTATES getState();
    void setState(NODESTATES);

    ProcessItem& getProcess();

};


#endif //DICON_NODEOBJECT_H
