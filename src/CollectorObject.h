//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COLLECTOROBJECT_H
#define DICON_COLLECTOROBJECT_H

#include "ComponentObject.h"
#include "CollectorState.h"

class CollectorObject : public ComponentObject {

private:

    COLLSTATES state;

    ComponentObject attachedNode;

public:

    CollectorObject(COLLSTATES, ComponentObject, int, long);
    CollectorObject(int, long);
    ~CollectorObject();

    COLLSTATES getState();
    void setState(COLLSTATES);

    ComponentObject getAttached();
    void setAttached(ComponentObject);
};


#endif //DICON_COLLECTOROBJECT_H
