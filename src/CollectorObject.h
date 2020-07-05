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

public:

    CollectorObject(COLLSTATES, int, long);
    CollectorObject(int, long);
    CollectorObject(const char *);
    CollectorObject();

    ~CollectorObject();

    COLLSTATES getState();
    void setState(COLLSTATES);
};


#endif //DICON_COLLECTOROBJECT_H
