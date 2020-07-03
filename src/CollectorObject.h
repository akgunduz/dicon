//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COLLECTOROBJECT_H
#define DICON_COLLECTOROBJECT_H

#include "NodeObject.h"
#include "CollectorState.h"

class CollectorObject : public ComponentObject {

private:

    COLLSTATES state;

    NodeObject attachedNode;

public:

    CollectorObject();
    CollectorObject(const char *);
    CollectorObject(int, long);
    CollectorObject(COLLSTATES, NodeObject, int, long);
    ~CollectorObject();

    COLLSTATES getState();
    void setState(COLLSTATES);

    NodeObject getAttached();
    void setAttached(NodeObject);
};


#endif //DICON_COLLECTOROBJECT_H
