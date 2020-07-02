//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_NODEOBJECT_H
#define DICON_NODEOBJECT_H

#include "NodeState.h"
#include "Md5.h"
#include "ComponentObject.h"

class NodeObject : public ComponentObject {

private:

    NODESTATES state;

    int usage;

public:

    NodeObject(NODESTATES, int, int, long);
    NodeObject(int, long);
    ~NodeObject();


    NODESTATES getState();
    void setState(NODESTATES);
    int getUsage();
    int iterateUsage(bool);
};


#endif //DICON_NODEOBJECT_H
