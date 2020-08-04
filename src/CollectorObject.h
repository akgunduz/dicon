//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COLLECTOROBJECT_H
#define DICON_COLLECTOROBJECT_H

#include "NodeHost.h"
#include "CollectorState.h"

class CollectorObject : public ComponentObject {

    COLLSTATES state{COLLSTATE_IDLE};

public:

    CollectorObject() = default;
    explicit CollectorObject(COLLSTATES);

    COLLSTATES getState();
    void setState(COLLSTATES);

};

#endif //DICON_COLLECTOROBJECT_H
