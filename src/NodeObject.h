//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_NODEOBJECT_H
#define BANKOR_NODEOBJECT_H

#include "NodeState.h"
#include "Md5.h"
#include "ComponentObject.h"

class NodeObject : public ComponentObject {

private:

    NODE_STATES state;
    int usage;

public:

    NodeObject(NODE_STATES, int, int, long);
    NodeObject(int, long);
    ~NodeObject();


    NODE_STATES getState();
    void setState(NODE_STATES);
    int getUsage();
    int iterateUsage(bool);
};


#endif //BANKOR_NODEOBJECT_H
