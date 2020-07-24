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

    CollectorObject(COLLSTATES, long, long);
    CollectorObject(long, long);
    CollectorObject(const char *);
    CollectorObject();
    CollectorObject(const CollectorObject &);
    CollectorObject(const ComponentObject &);

    ~CollectorObject();

    COLLSTATES getState();
    void setState(COLLSTATES);
};


#endif //DICON_COLLECTOROBJECT_H
